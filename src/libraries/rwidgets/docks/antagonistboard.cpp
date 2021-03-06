#include "antagonistboard.h"
#include "ui_antagonistboard.h"

#include <QBuffer>
#include <QByteArray>
#include <QDrag>
#include <QFileDialog>
#include <QJsonArray>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonValue>
#include <QMenu>

#include "controller/antagonistboardcontroller.h"
#include "data/campaign.h"
#include "data/campaigneditor.h"
#include "data/character.h"
#include "model/genericmodel.h"
#include "model/nonplayablecharactermodel.h"
#include "worker/utilshelper.h"

#include "controller/view_controller/imageselectorcontroller.h"
#include "data/rolisteammimedata.h"
#include "rwidgets/delegates/avatardelegate.h"
#include "rwidgets/delegates/taglistdelegate.h"
#include "rwidgets/dialogs/imageselectordialog.h"
#include "worker/iohelper.h"

namespace campaign
{
namespace
{
template <class T>
T* convertField(CharacterField* field)
{
    return dynamic_cast<T*>(field);
}

void setAllRowToHeight(GenericModel* model, int height, QTableView* view)
{
    for(int i= 0; i < model->rowCount(); ++i)
    {
        view->setRowHeight(i, height);
    }
}
} // namespace
AntagonistBoard::AntagonistBoard(campaign::CampaignEditor* editor, QWidget* parent)
    : QWidget(parent)
    , ui(new Ui::AntagonistBoard)
    , m_ctrl(new AntagonistBoardController(editor))
    , m_createTokenAct(new QAction(tr("Edit Token")))
    , m_cloneCharacterAct(new QAction(tr("Clone Character")))
    , m_changeImageAct(new QAction(tr("Change Image")))
    , m_fullModeAct(new QAction(tr("Main columns")))
    , m_saveTokenAct(new QAction(tr("Save")))
{
    ui->setupUi(this);

    ui->m_antogonistView->setModel(m_ctrl->filteredModel());
    ui->m_antogonistView->setDragEnabled(true);
    ui->m_antogonistView->setSelectionMode(QAbstractItemView::ExtendedSelection);
    ui->m_antogonistView->setSelectionBehavior(QAbstractItemView::SelectRows);
    ui->m_antogonistView->setContextMenuPolicy(Qt::CustomContextMenu);
    ui->m_antogonistView->setItemDelegateForColumn(NonPlayableCharacterModel::ColAvatar, new AvatarDelegate(this));
    ui->m_antogonistView->setItemDelegateForColumn(NonPlayableCharacterModel::ColTags, new TagListDelegate(this));
    connect(ui->m_antogonistView, &QTableView::customContextMenuRequested, this, &AntagonistBoard::contextualMenu);
    m_fullModeAct->setCheckable(true);
    ui->m_antogonistView->setIconSize(QSize(64, 64));

    ui->m_saveBtn->setDefaultAction(m_saveTokenAct.get());

    connect(m_saveTokenAct.get(), &QAction::triggered, this, [this] {
        auto character= m_ctrl->character();

        character->setName(ui->m_nameEdit->text());
        character->setSize(ui->m_sizeEdit->value());
        character->setInitCommand(ui->m_initCommand->text());
        character->setInitiativeScore(ui->m_initValue->value());
        character->setColor(ui->m_colorSelector->color());
        character->setHealthPointsCurrent(ui->m_currentLife->value());
        character->setHealthPointsMax(ui->m_maxLife->value());
        character->setHealthPointsMin(ui->m_lifeMin->value());
        character->setAvatar(IOHelper::pixmapToData(ui->m_avatarDisplay->pixmap()));

        m_ctrl->saveToken();
        m_ctrl->setCharacter(nullptr);
    });

    ui->m_antogonistView->installEventFilter(this);

    connect(m_fullModeAct.get(), &QAction::toggled, this, [this]() {
        bool showMainCols= m_fullModeAct->isChecked();
        int i= 0;
        for(auto const& act : m_columnsAction)
        {
            act->setEnabled(!showMainCols);
            if(i < 4)
            {
                ui->m_antogonistView->showColumn(i);
            }
            else if(showMainCols)
            {
                ui->m_antogonistView->hideColumn(i);
            }
            else if(act->isChecked())
            {
                ui->m_antogonistView->showColumn(i);
            }
            ++i;
        }
    });

    if(!editor)
        return;

    auto cols= editor->campaign()->npcModel()->headers();

    int i= 0;
    std::for_each(std::begin(cols), std::end(cols), [&i, this](const QString& colname) {
        std::unique_ptr<QAction> act(new QAction(colname));
        act->setCheckable(true);
        act->setChecked(true);
        connect(act.get(), &QAction::triggered, this, &AntagonistBoard::hideColumn);
        act->setData(i);
        ++i;
        m_columnsAction.push_back(std::move(act));
    });

    connect(ui->m_addCharacterAct, &QAction::triggered, this, [this]() { m_ctrl->addCharacter(); });
    connect(ui->m_removeCharacterAct, &QAction::triggered, this, [this]() {
        auto index= ui->m_antogonistView->currentIndex();
        if(!index.isValid())
            return;

        m_ctrl->removeCharacter(index.data(NonPlayableCharacterModel::RoleUuid).toString());
    });
    m_fullModeAct->setChecked(true);

    ui->m_addCharacterBtn->setDefaultAction(ui->m_addCharacterAct);
    ui->m_removeCharacterBtn->setDefaultAction(ui->m_removeCharacterAct);

    ui->m_addPropertyBtn->setDefaultAction(ui->m_addPropertyAct);
    ui->m_addActionBtn->setDefaultAction(ui->m_addActionAct);
    ui->m_addShapeBtn->setDefaultAction(ui->m_addShapeAct);

    ui->m_removeActBtn->setDefaultAction(ui->m_removeActionAct);
    ui->m_removeShapeBtn->setDefaultAction(ui->m_removeShapeAct);
    ui->m_removePropertyBtn->setDefaultAction(ui->m_removePropertyAct);

    connect(ui->m_antogonistView, &DragableTableView::dragItem, this, [this](QModelIndex index) {
        if(index.column() != NonPlayableCharacterModel::ColAvatar)
            return;
        auto uuid= index.data(NonPlayableCharacterModel::RoleUuid).toString();
        auto name= index.data(NonPlayableCharacterModel::RoleName).toString();
        auto img= index.data(NonPlayableCharacterModel::RoleAvatar);

        if(img.isNull())
            return;
        QDrag* drag= new QDrag(this);
        RolisteamMimeData* mimeData= new RolisteamMimeData();

        mimeData->setNpcUuid(uuid);
        mimeData->setImageData(img);
        mimeData->setText(name);
        drag->setMimeData(mimeData);
        drag->setPixmap(helper::utils::roundCornerImage(qvariant_cast<QPixmap>(img)));
        drag->exec();
    });

    connect(m_createTokenAct.get(), &QAction::triggered, this, [this]() {
        if(m_currentItemId.isEmpty())
            return;
        m_ctrl->editCharacter(m_currentItemId);
    });

    connect(m_changeImageAct.get(), &QAction::triggered, m_ctrl.get(), [this]() {
        ImageSelectorController ctrl(false, ImageSelectorController::All, ImageSelectorController::Square);
        ImageSelectorDialog dialog(&ctrl, this);
        if(QDialog::Accepted != dialog.exec())
            return;

        m_ctrl->changeImage(m_currentItemId, ctrl.finalImageData());
    });

    connect(m_ctrl.get(), &AntagonistBoardController::characterChanged, this, [this]() {
        if(m_ctrl->editingCharacter())
        {
            auto character= m_ctrl->character();
            ui->m_nameEdit->setText(character->name());
            ui->m_colorSelector->setColor(character->getColor());
            ui->m_lifeMin->setValue(character->getHealthPointsMin());
            ui->m_maxLife->setValue(character->getHealthPointsMax());
            ui->m_currentLife->setValue(character->getHealthPointsCurrent());
            ui->m_initCommand->setText(character->initCommand());
            ui->m_initValue->setValue(character->getInitiativeScore());
            ui->m_avatarDisplay->setPixmap(IOHelper::dataToPixmap(character->avatar()));
            ui->stackedWidget->setCurrentIndex(1);
        }
        else
        {
            ui->stackedWidget->setCurrentIndex(0);
        }
    });
    ui->stackedWidget->setCurrentIndex(0);

    ui->m_actionList->setModel(m_ctrl->actionModel());
    ui->m_propertyList->setModel(m_ctrl->propertyModel());
    ui->m_shapeList->setModel(m_ctrl->shapeModel());

    connect(ui->m_addActionAct, &QAction::triggered, this,
            [this]() { m_ctrl->appendData(campaign::ModelType::Model_Action); });
    connect(ui->m_addPropertyAct, &QAction::triggered, this,
            [this]() { m_ctrl->appendData(campaign::ModelType::Model_Properties); });
    connect(ui->m_addActionAct, &QAction::triggered, this, [this]() {
        m_ctrl->appendData(campaign::ModelType::Model_Shape);
        setAllRowToHeight(m_ctrl->shapeModel(), ui->m_sizeEdit->value(), ui->m_shapeList);
    });

    ui->m_removeActionAct->setData(QVariant::fromValue(campaign::ModelType::Model_Action));
    ui->m_removePropertyAct->setData(QVariant::fromValue(campaign::ModelType::Model_Properties));
    ui->m_removeShapeAct->setData(QVariant::fromValue(campaign::ModelType::Model_Shape));

    auto remove= [this]() {
        auto act= qobject_cast<QAction*>(sender());
        auto mode= act->data().value<campaign::ModelType>();
        QModelIndex index;
        switch(mode)
        {
        case campaign::ModelType::Model_Action:
            index= ui->m_actionList->currentIndex();
            break;
        case campaign::ModelType::Model_Shape:
            index= ui->m_shapeList->currentIndex();
            break;
        case campaign::ModelType::Model_Properties:
            index= ui->m_propertyList->currentIndex();
            break;
        }
        m_ctrl->removeData(index, mode);
    };

    connect(ui->m_removeActionAct, &QAction::triggered, this, remove);
    connect(ui->m_removePropertyAct, &QAction::triggered, this, remove);
    connect(ui->m_removeShapeAct, &QAction::triggered, this, remove);

    connect(ui->m_shapeList, &QTableView::doubleClicked, this, [this](const QModelIndex& index) {
        auto path= index.data().toString();
        auto dir= QDir::homePath();

        if(!path.isEmpty())
        {
            QFileInfo info(path);
            dir= info.absolutePath();
        }

        ImageSelectorController ctrl(false, ImageSelectorController::All, ImageSelectorController::Square);
        ImageSelectorDialog dialog(&ctrl, this);
        if(QDialog::Accepted != dialog.exec())
            return;

        auto data= ctrl.finalImageData();
        // m_shapeModel->setData(index, data);
    });

    connect(ui->m_sizeEdit, QOverload<int>::of(&QSpinBox::valueChanged), this, [=] {
        auto val= ui->m_sizeEdit->value();
        ui->m_shapeList->setIconSize(QSize(val, val));
        // setAllRowToHeight(m_shapeModel, val, ui->m_shapeList);
    });

    // sconnect(ui->m_avatarEdit, &QLineEdit::textChanged, this, &AntagonistBoard::updateImage);
    connect(ui->m_avatarOpenFileBtn, &QPushButton::clicked, this, [this]() {
        ImageSelectorController ctrl(true, ImageSelectorController::All, ImageSelectorController::Square);
        ImageSelectorDialog dialog(&ctrl, this);
        if(QDialog::Accepted != dialog.exec())
            return;

        auto data= ctrl.finalImageData();
        updateImage(data);
    });
#ifndef Q_OS_MAC
    ui->m_actionList->setAlternatingRowColors(true);
    ui->m_shapeList->setAlternatingRowColors(true);
    ui->m_propertyList->setAlternatingRowColors(true);
#endif
}

AntagonistBoard::~AntagonistBoard()
{
    delete ui;
}

void AntagonistBoard::contextualMenu(const QPoint& pos)
{
    QMenu menu;

    auto index= ui->m_antogonistView->indexAt(pos);

    m_currentItemId= index.isValid() ? index.data(NonPlayableCharacterModel::RoleUuid).toString() : QString();
    qDebug() << "index contextual menu:" << index.row() << m_currentItemId;
    menu.addAction(ui->m_addCharacterAct);
    menu.addAction(m_cloneCharacterAct.get());
    menu.addAction(ui->m_removeCharacterAct);
    menu.addAction(m_createTokenAct.get());
    menu.addAction(m_changeImageAct.get());
    menu.addSeparator();
    menu.addAction(m_fullModeAct.get());
    auto colsMenu= menu.addMenu(tr("Show Columnns"));
    for(auto const& act : m_columnsAction)
    {
        colsMenu->addAction(act.get());
    }

    m_createTokenAct->setEnabled(index.isValid());
    m_cloneCharacterAct->setEnabled(index.isValid());
    ui->m_removeCharacterAct->setEnabled(index.isValid());
    m_changeImageAct->setEnabled(index.isValid());

    menu.exec(ui->m_antogonistView->mapToGlobal(pos));
}

void AntagonistBoard::updateImage(const QByteArray& data)
{
    QPixmap pix;
    pix.loadFromData(data);
    ui->m_avatarDisplay->setPixmap(pix);
}

void AntagonistBoard::hideColumn()
{
    auto act= qobject_cast<QAction*>(sender());
    auto i= act->data().toInt();

    if(act->isChecked())
        ui->m_antogonistView->showColumn(i);
    else
        ui->m_antogonistView->hideColumn(i);
}

bool AntagonistBoard::eventFilter(QObject* obj, QEvent* event)
{
    if(obj == ui->m_antogonistView)
    {
        /*if(event->type() == QEvent::MouseMove)
        {
            auto e= dynamic_cast<QMouseEvent*>(event);
            if(e)
            {
                auto index= ui->m_antogonistView->indexAt(e->pos());
            }
        }*/
    }

    return QWidget::eventFilter(obj, event);
}
} // namespace campaign
