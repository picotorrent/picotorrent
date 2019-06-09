#include "proxysectionwidget.hpp"

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

using pt::ProxySectionWidget;

ProxySectionWidget::ProxySectionWidget()
{
    createUi();

    connect(
        m_proxyType,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ProxySectionWidget::onProxyTypeChanged);
}

void ProxySectionWidget::loadConfig(std::shared_ptr<pt::Configuration> cfg)
{
    int proxyType = cfg->getInt("proxy_type");

    for (int i = 0; i < m_proxyType->count(); i++)
    {
        if (proxyType == m_proxyType->itemData(i).toInt())
        {
            m_proxyType->setCurrentIndex(i);
            onProxyTypeChanged(i);
            break;
        }
    }

    int proxyPort = cfg->getInt("proxy_port");

    m_proxyHost->setText(QString::fromStdString(cfg->getString("proxy_host")));
    m_proxyPort->setText(proxyPort > 0 ? QString::number(proxyPort) : "");
    m_proxyUsername->setText(QString::fromStdString(cfg->getString("proxy_username")));
    m_proxyPassword->setText(QString::fromStdString(cfg->getString("proxy_password")));
    m_proxyHostnames->setChecked(cfg->getBool("proxy_hostnames"));
    m_proxyPeerConnections->setChecked(cfg->getBool("proxy_peers"));
    m_proxyTrackerConnections->setChecked(cfg->getBool("proxy_trackers"));
}

void ProxySectionWidget::saveConfig(std::shared_ptr<pt::Configuration> cfg)
{
    auto typeIndex = m_proxyType->currentIndex();
    auto typeData = m_proxyType->itemData(typeIndex);

    cfg->setInt("proxy_type", typeData.toInt());
    cfg->setString("proxy_host", m_proxyHost->text().toStdString());
    cfg->setInt("proxy_port", m_proxyPort->text().toInt());
    cfg->setString("proxy_username", m_proxyUsername->text().toStdString());
    cfg->setString("proxy_password", m_proxyPassword->text().toStdString());
    cfg->setBool("proxy_hostnames", m_proxyHostnames->checkState() == Qt::Checked);
    cfg->setBool("proxy_peers", m_proxyPeerConnections->checkState() == Qt::Checked);
    cfg->setBool("proxy_trackers", m_proxyTrackerConnections->checkState() == Qt::Checked);
}

void ProxySectionWidget::createUi()
{
    m_proxyType = new QComboBox();
    m_proxyHost = new QLineEdit();
    m_proxyPort = new QLineEdit();
    m_proxyUsername = new QLineEdit();
    m_proxyPassword = new QLineEdit();
    m_proxyHostnames = new QCheckBox(i18n("proxy_hostnames"));
    m_proxyPeerConnections = new QCheckBox(i18n("proxy_peer_connections"));
    m_proxyTrackerConnections = new QCheckBox(i18n("proxy_tracker_connections"));

    m_proxyPort->setAlignment(Qt::AlignRight);
    m_proxyPort->setMaximumWidth(50);
    m_proxyPort->setValidator(new QIntValidator());

    m_proxyPassword->setEchoMode(QLineEdit::Password);

    m_proxyType->addItem(i18n("none"), static_cast<int>(Configuration::ConnectionProxyType::None));
    m_proxyType->addItem(i18n("http"), static_cast<int>(Configuration::ConnectionProxyType::HTTP));
    m_proxyType->addItem(i18n("http_with_credentials"), static_cast<int>(Configuration::ConnectionProxyType::HTTP_Password));
    m_proxyType->addItem(i18n("socks4"), static_cast<int>(Configuration::ConnectionProxyType::SOCKS4));
    m_proxyType->addItem(i18n("socks5"), static_cast<int>(Configuration::ConnectionProxyType::SOCKS5));
    m_proxyType->addItem(i18n("socks5_with_credentials"), static_cast<int>(Configuration::ConnectionProxyType::SOCKS5_Password));

    auto proxyLayout = new QGridLayout();
    proxyLayout->addWidget(new QLabel(i18n("type")), 0, 0);
    proxyLayout->addWidget(m_proxyType, 0, 1);
    proxyLayout->addWidget(new QLabel(i18n("host")), 1, 0);
    proxyLayout->addWidget(m_proxyHost, 1, 1);
    proxyLayout->addWidget(new QLabel(i18n("port")), 2, 0);
    proxyLayout->addWidget(m_proxyPort, 2, 1);
    proxyLayout->addWidget(new QLabel(i18n("username")), 3, 0);
    proxyLayout->addWidget(m_proxyUsername, 3, 1);
    proxyLayout->addWidget(new QLabel(i18n("password")), 4, 0);
    proxyLayout->addWidget(m_proxyPassword, 4, 1);

    proxyLayout->addWidget(m_proxyHostnames, 5, 0);
    proxyLayout->addWidget(m_proxyPeerConnections, 5, 1);
    proxyLayout->addWidget(m_proxyTrackerConnections, 6, 0);

    auto proxyGroup = new QGroupBox(i18n("proxy"));
    proxyGroup->setLayout(proxyLayout);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(proxyGroup);
    layout->addStretch();
    layout->setMargin(0);

    this->setLayout(layout);
}

void ProxySectionWidget::onProxyTypeChanged(int index)
{
    auto type = static_cast<Configuration::ConnectionProxyType>(m_proxyType->itemData(index).toInt());

    m_proxyHost->setEnabled(false);
    m_proxyPort->setEnabled(false);
    m_proxyUsername->setEnabled(false);
    m_proxyPassword->setEnabled(false);
    m_proxyHostnames->setEnabled(false);
    m_proxyPeerConnections->setEnabled(false);
    m_proxyTrackerConnections->setEnabled(false);

    switch(type)
    {
    case Configuration::ConnectionProxyType::HTTP:
    case Configuration::ConnectionProxyType::HTTP_Password:
    case Configuration::ConnectionProxyType::SOCKS4:
    case Configuration::ConnectionProxyType::SOCKS5:
    case Configuration::ConnectionProxyType::SOCKS5_Password:
    {
        m_proxyHost->setEnabled(true);
        m_proxyPort->setEnabled(true);
        m_proxyHostnames->setEnabled(true);
        m_proxyPeerConnections->setEnabled(true);
        m_proxyTrackerConnections->setEnabled(true);
        break;
    }
    }

    switch(type)
    {
    case Configuration::ConnectionProxyType::HTTP_Password:
    case Configuration::ConnectionProxyType::SOCKS5_Password:
    {
        m_proxyUsername->setEnabled(true);
        m_proxyPassword->setEnabled(true);
        break;
    }
    }
}
