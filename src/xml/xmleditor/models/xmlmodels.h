#ifndef XMLMODELS_H
#define XMLMODELS_H

#include "xmlmodel.h"
#include "xmlsgroupmodel.h"

class XmlBaseModel : public XmlModel
{
public:
    explicit XmlBaseModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual QDomElement *toNode(QDomDocument &doc) override;
};

/// SIGNALS

class XmlSignalsModel : public XmlModel
{
public:
    explicit XmlSignalsModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual QDomElement *toNode(QDomDocument &doc) override;
};

/// SECTIONS

class XmlSectionTabsModel : public XmlModel
{
public:
    explicit XmlSectionTabsModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/* Some models namings explanation. Lets see the structure of section part in configuration XML-file:
 * <sections desc="Sections">                       <-- XmlSectionsModel, contains XmlSectionModel-s
 *      <section header="Section header">           <-- XmlSectionModel, contains XmlSGroupModel-s
 *          <sgroup header="Section group header">  <-- XmlSGroupModel, doesn't contains models, only data
 *              <mwidget desc="Widget desc">
 *                  ...
 *                  # Some inner data...
 *                  ...
 *              </mwidget>
 *              ...
 *              # Other multi-widgets...
 *              ...
 *          </sgroup>
 *          ...
 *          # Other section groups...
 *          ...
 *      </section>
 *      ...
 *      # Other sections...
 *      ...
 * </sections>
 *
 * Some dialogs namings explanation:
 * 1) XmlSectionsModel contains sections. When user selects a item - user selects a section.
 * Therefore create/edit dialog naming in this case is XmlSectionDialog.
 * 2) XmlSectionModel contains section groups. When user selects a item - user selects a section group.
 * Therefore create/edit dialog naming in this case is XmlSGroupDialog.
 * 3) XmlSGroupModel contains multi-widgets. When user selects a item - user selects a multi-widget.
 * Therefore create/edit dialog naming in this case is XmlMWidgetDialog.
 */

class XmlSectionsModel : public XmlModel
{
public:
    explicit XmlSectionsModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual void create(const QStringList &saved, int *row) override;
};

class XmlSectionModel : public XmlModel
{
public:
    explicit XmlSectionModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
    virtual void create(const QStringList &saved, int *row) override;
};

/// ALARMS

class XmlAlarmsModel : public XmlModel
{
public:
    explicit XmlAlarmsModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// JOURNALS

class XmlWorkJoursModel : public XmlModel
{
public:
    explicit XmlWorkJoursModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlMeasJoursModel : public XmlModel
{
public:
    explicit XmlMeasJoursModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// DATA EXCHANGE PROTOCOLS

class XmlModbusModel : public XmlModel
{
public:
    explicit XmlModbusModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlProtocomModel : public XmlModel
{
public:
    explicit XmlProtocomModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

class XmlIecModel : public XmlModel
{
public:
    explicit XmlIecModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

/// S2 CONFIG

class XmlConfigModel : public XmlModel
{
public:
    explicit XmlConfigModel(int rows, int cols, ModelType type, QObject *parent = nullptr);
    virtual void parseNode(QDomNode &node, int &row) override;
};

#endif // XMLMODELS_H
