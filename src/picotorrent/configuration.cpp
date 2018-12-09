#include "configuration.hpp"

#include "database.hpp"

using pt::Configuration;

Configuration::Configuration(std::shared_ptr<pt::Database> db)
    : m_db(db)
{
}

Configuration::~Configuration()
{
}

bool Configuration::getBool(std::string const& key)
{
    return false;
}

int Configuration::getInt(std::string const& key)
{
    return 0;
}

std::string Configuration::getString(std::string const& key)
{
    auto stmt = m_db->statement("select string_value from setting where key = ?");
    stmt->bind(1, key);
    stmt->execute();

    return stmt->getString(0);
}


void Configuration::setBool(std::string const& key, bool value)
{
}

void Configuration::setInt(std::string const& key, int value)
{
}

void Configuration::setString(std::string const& key, std::string const& value)
{
}

