#ifndef DICEBOOKMARKMODEL_H
#define DICEBOOKMARKMODEL_H

#include "data/diceshortcut.h"
#include "rwidgets_global.h"
#include <QAbstractTableModel>
#include <QSettings>
/**
 * @brief The DiceBookMarkModel class
 */
class RWIDGET_EXPORT DiceBookMarkModel : public QAbstractTableModel
{
    Q_OBJECT

public:
    explicit DiceBookMarkModel(std::vector<DiceShortCut>& data, QObject* parent= nullptr);

    // Header:
    QVariant headerData(int section, Qt::Orientation orientation, int role= Qt::DisplayRole) const override;

    // Basic functionality:
    int rowCount(const QModelIndex& parent= QModelIndex()) const override;
    int columnCount(const QModelIndex& parent= QModelIndex()) const override;

    QVariant data(const QModelIndex& index, int role= Qt::DisplayRole) const override;

    // Editable:
    bool setData(const QModelIndex& index, const QVariant& value, int role= Qt::EditRole) override;

    Qt::ItemFlags flags(const QModelIndex& index) const override;

    // Add data:
    bool appendRows();

    // Remove data:
    bool removeRows(int row, int count, const QModelIndex& parent= QModelIndex()) override;

    QMimeData* mimeData(const QModelIndexList& indexes) const override;
    QStringList mimeTypes() const override;

    void writeSettings(QSettings& settings);
    void readSettings(QSettings& settings);

private:
    std::vector<DiceShortCut>& m_data;
};

#endif // DICEBOOKMARKMODEL_H
