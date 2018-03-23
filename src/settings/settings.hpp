#ifndef SETTINGS_H
#define SETTINGS_H

#include <cassert>
#include <map>
#include <memory>
#include <string>

#include <iostream>

namespace samsar {
    class SettingsInterface;
    template <typename T> class SettingsField;

    using SettingsInterfacePtr = std::shared_ptr<SettingsInterface>;
    template <typename T> using SettingsFieldPtr = std::shared_ptr<SettingsField<T>>;

    class SettingsInterface {
    public:
        ~SettingsInterface() = default;
    };

    template <typename T> class SettingsField : public SettingsInterface {
    public:
        SettingsField() {}
        SettingsField(const T& field) : _field(field) {}

        T& value() { return _field; }
        T value() const { return _field; }

    private:
        T _field;
    };

    class Settings {
    public:
        Settings();
        ~Settings() = default;

        const std::map<std::string, SettingsInterfacePtr>& operator()();
        SettingsInterfacePtr operator()(const std::string& setting);

        template <typename T> SettingsFieldPtr<T> operator()(const std::string& setting)
        {
            return get_field<T>(setting);
        }

        template <typename T> SettingsFieldPtr<T> get_field(const std::string& setting)
        {
            if (_settings.find(setting) == _settings.end())
                return nullptr;
            else
                return std::static_pointer_cast<SettingsField<T>>(_settings[setting]);
        }

        template <typename T> SettingsFieldPtr<T> get_field(const std::string& setting) const
        {
            return std::static_pointer_cast<SettingsField<T>>(_settings.at(setting));
        }

        template <typename T> Settings& add_setting(const std::string& setting, T field)
        {
            SettingsFieldPtr<T> f = std::make_shared<SettingsField<T>>(field);
            _settings[setting] = f;
            return *this;
        }

        size_t size() const;

    private:
        std::map<std::string, SettingsInterfacePtr> _settings;
    };

} // namespace samsar

#endif
