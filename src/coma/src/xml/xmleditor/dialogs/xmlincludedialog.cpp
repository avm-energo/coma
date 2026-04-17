#include "xml/xmleditor/dialogs/xmlincludedialog.h"

#include <avm-gen/settings.h>
#include <avm-widgets/filefunc.h>

#include <QDir>
#include <QFileInfo>

namespace
{
constexpr auto kSrcEditObjectName = "xmlIncludeSrcEdit";
constexpr auto kSrcFileMask = "XML-файлы (*.xml);;Все файлы (*)";
} // namespace

XmlIncludeDialog::XmlIncludeDialog(QWidget *parent) : XmlDialog(parent)
{
}

void XmlIncludeDialog::setupUI(QVBoxLayout *mainLayout)
{
    setupSizePos(600, 100);
    m_title += "ссылки на внешний XML-файл";

    // Файловый диалог в FileFunc открывается из Settings::workDir().
    // Для нашего случая это директория открытых XML-файлов модулей (configDir).
    Settings::setWorkDir(Settings::configDir());

    // Виджет выбора файла (Label + LineEdit + кнопка "...").
    auto fileWidget
        = FileFunc::newLBL(this, "Путь к файлу: ", kSrcEditObjectName, FileFunc::FILE, kSrcFileMask, QString());
    mainLayout->addWidget(fileWidget);

    // Внутренний QLineEdit, создаваемый FileFunc, регистрируем в m_dlgItems,
    // чтобы collectData/loadModelData работали как и для других диалогов.
    auto srcInput = findChild<QLineEdit *>(kSrcEditObjectName);
    if (srcInput != nullptr)
    {
        // textChanged ловит и ручной ввод, и программную установку после выбора файла.
        QObject::connect(
            srcInput, &QLineEdit::textChanged, this, qOverload<const QString &>(&XmlIncludeDialog::dataChanged));
        m_dlgItems.append(srcInput);
    }
}

QStringList XmlIncludeDialog::collectData()
{
    auto collected = XmlDialog::collectData();
    if (!collected.isEmpty())
    {
        // После выбора через QFileDialog в поле лежит абсолютный путь.
        // Приводим его к относительному относительно configDir, чтобы в XML
        // хранились короткие, переносимые пути (parser тоже резолвит относительно configDir).
        const QDir baseDir(Settings::configDir());
        const QFileInfo info(collected.first());
        if (info.isAbsolute())
        {
            const auto rel = baseDir.relativeFilePath(info.absoluteFilePath());
            if (!rel.isEmpty() && !rel.startsWith(".."))
                collected[0] = rel;
        }
    }
    return collected;
}
