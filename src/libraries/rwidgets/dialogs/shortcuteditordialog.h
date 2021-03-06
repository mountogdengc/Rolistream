#ifndef SHORTCUTEDITORDIALOG_H
#define SHORTCUTEDITORDIALOG_H

#include "rwidgets_global.h"
#include <QDialog>
class ShortCutModel;
namespace Ui
{
class ShortCutEditorDialog;
}
class QAbstractItemModel;
class RWIDGET_EXPORT ShortCutEditorDialog : public QDialog
{
    Q_OBJECT

public:
    explicit ShortCutEditorDialog(QWidget* parent= nullptr);
    ~ShortCutEditorDialog();

    QAbstractItemModel* model() const;
    void setModel(QAbstractItemModel* model);

private:
    Ui::ShortCutEditorDialog* ui;
    QAbstractItemModel* m_model;
};

#endif // SHORTCUTEDITORDIALOG_H
