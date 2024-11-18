#include <QFile>
#include <QJsonDocument>
#include <QJsonObject>
#include <QJsonArray>
#include <QDebug>
#include <yaml-cpp/yaml.h>
QJsonValue yamlNodeToJsonValue(const YAML::Node &node)
{
    switch (node.Type())
    {
    case YAML::NodeType::Scalar:
    {
        if (node.IsScalar())
        {
            const auto &scalar = node.as<std::string>();
            // 尝试转换为bool
            if (scalar == "true" || scalar == "false")
            {
                return QJsonValue(scalar == "true");
            }
            // 尝试转换为double
            try
            {
                double doubleValue = node.as<double>();
                return QJsonValue(doubleValue);
            }
            catch (const YAML::TypedBadConversion<double> &)
            {
                // 转换失败，忽略异常
            }
            // 尝试转换为int
            try
            {
                int intValue = node.as<int>();
                return QJsonValue(intValue);
            }
            catch (const YAML::TypedBadConversion<int> &)
            {
                // 转换失败，忽略异常
            }
            // 默认返回字符串
            return QJsonValue(QString::fromStdString(scalar));
        }
        break;
    }
    case YAML::NodeType::Sequence:
    {
        QJsonArray array;
        for (const auto &element : node)
        {
            array.append(yamlNodeToJsonValue(element));
        }
        return QJsonValue(array);
    }
    case YAML::NodeType::Map:
    {
        QJsonObject object;
        for (const auto &pair : node)
        {
            object[QString::fromStdString(pair.first.as<std::string>())] = yamlNodeToJsonValue(pair.second);
        }
        return QJsonValue(object);
    }
    default:
        break;
    }
    return QJsonValue();
}
int main(int argc, char *argv[])
{
    QFile yamlFile("/home/ray/myGitproject/my-case/linux/tar/carbon_block_clean/config/template_config.yaml");
    if (!yamlFile.open(QIODevice::ReadOnly | QIODevice::Text))
    {
        qWarning("Failed to open YAML file.");
        return -1;
    }

    YAML::Node yamlDoc = YAML::Load(yamlFile.readAll().toStdString());
    yamlFile.close();

    QJsonObject jsonObject = yamlNodeToJsonValue(yamlDoc).toObject();

    QJsonDocument jsonDoc(jsonObject);
    QFile jsonFile("/home/ray/myGitproject/my-case/linux/tar/carbon_block_clean/config/recv.json");
    if (!jsonFile.open(QIODevice::WriteOnly))
    {
        qWarning("Failed to open JSON file for writing.");
        return -1;
    }

    jsonFile.write(jsonDoc.toJson());
    jsonFile.close();
}