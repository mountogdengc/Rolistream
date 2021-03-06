#ifndef SETPROPERTYONALLCHARACTERCOMMAND_H
#define SETPROPERTYONALLCHARACTERCOMMAND_H

#include "charactersheet/charactersheet.h"
#include "charactersheet/charactersheetmodel.h"
#include <QUndoCommand>

class SetPropertyOnCharactersCommand : public QUndoCommand
{
public:
    SetPropertyOnCharactersCommand(QString idProperty, QString value, QString formula, CharacterSheetModel* model,
                                   QUndoCommand* parent= 0);

    void undo() override;
    void redo() override;

private:
    CharacterSheetModel* m_model;
    QString m_value;
    QString m_formula;
    QString m_propertyId;
    QStringList m_oldValues;
    QStringList m_oldFormula;
};

#endif // SETFIELDPROPERTYCOMMAND_H
