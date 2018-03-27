#include "settings.hpp"

namespace samsar {

    Settings::Settings() {}

    const std::map<std::string, SettingsInterfacePtr>& Settings::operator()() { return _settings; }

    std::shared_ptr<SettingsInterface> Settings::operator()(const std::string& setting)
    {
        if (_settings.find(setting) == _settings.end())
            return nullptr;
        else
            return _settings[setting];
    }

    size_t Settings::size() const { return _settings.size(); }

    const std::map<std::string, SettingsInterfacePtr>& Settings::get_map() const
    {
        return _settings;
    }

} // namespace samsar
