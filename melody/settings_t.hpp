#ifndef SETTINGS_T_HPP
#define SETTINGS_T_HPP


#include <QStandardPaths>
#include <QDir>


struct settings_t{
    inline static const QString project_name = "melody";

    QDir config_dir{QDir{QStandardPaths::standardLocations(QStandardPaths::ConfigLocation)[0]}.absoluteFilePath(project_name)};
};


#endif
