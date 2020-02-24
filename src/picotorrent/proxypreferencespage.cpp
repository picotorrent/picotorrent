#include "proxypreferencespage.hpp"
#include "ui_proxypreferencespage.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include "core/configuration.hpp"
#include "translator.hpp"

using pt::ProxyPreferencesPage;

ProxyPreferencesPage::ProxyPreferencesPage(QWidget* parent)
    : QWidget(parent),
    m_ui(new Ui::ProxyPreferencesPage())
{
    m_ui->setupUi(this);

    // Translate
    m_ui->proxyGroup->setTitle(i18n("proxy"));
    m_ui->typeLabel->setText(i18n("type"));
    m_ui->hostLabel->setText(i18n("host"));
    m_ui->portLabel->setText(i18n("port"));
    m_ui->usernameLabel->setText(i18n("username"));
    m_ui->passwordLabel->setText(i18n("password"));
    m_ui->proxyHostnames->setText(i18n("proxy_hostnames"));
    m_ui->proxyPeerConnections->setText(i18n("proxy_peer_connections"));
    m_ui->proxyTrackerConnections->setText(i18n("proxy_tracker_connections"));

    // Add data
    m_ui->type->addItem(i18n("none"), static_cast<int>(Configuration::ConnectionProxyType::None));
    m_ui->type->addItem(i18n("http"), static_cast<int>(Configuration::ConnectionProxyType::HTTP));
    m_ui->type->addItem(i18n("http_with_credentials"), static_cast<int>(Configuration::ConnectionProxyType::HTTP_Password));
    m_ui->type->addItem(i18n("socks4"), static_cast<int>(Configuration::ConnectionProxyType::SOCKS4));
    m_ui->type->addItem(i18n("socks5"), static_cast<int>(Configuration::ConnectionProxyType::SOCKS5));
    m_ui->type->addItem(i18n("socks5_with_credentials"), static_cast<int>(Configuration::ConnectionProxyType::SOCKS5_Password));

    connect(
        m_ui->type,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ProxyPreferencesPage::onProxyTypeChanged);
}

ProxyPreferencesPage::~ProxyPreferencesPage()
{
    delete m_ui;
}

void ProxyPreferencesPage::loadConfig(std::shared_ptr<pt::Configuration> cfg)
{
    int proxyType = cfg->getInt("proxy_type");

    for (int i = 0; i < m_ui->type->count(); i++)
    {
        if (proxyType == m_ui->type->itemData(i).toInt())
        {
            m_ui->type->setCurrentIndex(i);
            onProxyTypeChanged(i);
            break;
        }
    }

    int proxyPort = cfg->getInt("proxy_port");

    m_ui->host->setText(QString::fromStdString(cfg->getString("proxy_host")));

    if (proxyPort == 0)
    {
        m_ui->port->clear();
    }
    else
    {
        m_ui->port->setValue(proxyPort);
    }

    m_ui->username->setText(QString::fromStdString(cfg->getString("proxy_username")));
    m_ui->password->setText(QString::fromStdString(cfg->getString("proxy_password")));
    m_ui->proxyHostnames->setChecked(cfg->getBool("proxy_hostnames"));
    m_ui->proxyPeerConnections->setChecked(cfg->getBool("proxy_peers"));
    m_ui->proxyTrackerConnections->setChecked(cfg->getBool("proxy_trackers"));
}

void ProxyPreferencesPage::saveConfig(std::shared_ptr<pt::Configuration> cfg)
{
    auto typeIndex = m_ui->type->currentIndex();
    auto typeData = m_ui->type->itemData(typeIndex);

    cfg->setInt("proxy_type", typeData.toInt());
    cfg->setString("proxy_host", m_ui->host->text().toStdString());
    cfg->setInt("proxy_port", m_ui->port->value());
    cfg->setString("proxy_username", m_ui->username->text().toStdString());
    cfg->setString("proxy_password", m_ui->password->text().toStdString());
    cfg->setBool("proxy_hostnames", m_ui->proxyHostnames->checkState() == Qt::Checked);
    cfg->setBool("proxy_peers", m_ui->proxyPeerConnections->checkState() == Qt::Checked);
    cfg->setBool("proxy_trackers", m_ui->proxyTrackerConnections->checkState() == Qt::Checked);
}

void ProxyPreferencesPage::onProxyTypeChanged(int index)
{
    auto type = static_cast<Configuration::ConnectionProxyType>(m_ui->type->itemData(index).toInt());

    m_ui->host->setEnabled(false);
    m_ui->port->setEnabled(false);
    m_ui->username->setEnabled(false);
    m_ui->password->setEnabled(false);
    m_ui->proxyHostnames->setEnabled(false);
    m_ui->proxyPeerConnections->setEnabled(false);
    m_ui->proxyTrackerConnections->setEnabled(false);

    switch(type)
    {
    case Configuration::ConnectionProxyType::HTTP:
    case Configuration::ConnectionProxyType::HTTP_Password:
    case Configuration::ConnectionProxyType::SOCKS4:
    case Configuration::ConnectionProxyType::SOCKS5:
    case Configuration::ConnectionProxyType::SOCKS5_Password:
    {
        m_ui->host->setEnabled(true);
        m_ui->port->setEnabled(true);
        m_ui->proxyHostnames->setEnabled(true);
        m_ui->proxyPeerConnections->setEnabled(true);
        m_ui->proxyTrackerConnections->setEnabled(true);
        break;
    }
    }

    switch(type)
    {
    case Configuration::ConnectionProxyType::HTTP_Password:
    case Configuration::ConnectionProxyType::SOCKS5_Password:
    {
        m_ui->username->setEnabled(true);
        m_ui->password->setEnabled(true);
        break;
    }
    }
}
