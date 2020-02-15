#include "connectionpreferencespage.hpp"
#include "ui_connectionpreferencespage.h"

#include <QCheckBox>
#include <QComboBox>
#include <QGridLayout>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QIntValidator>
#include <QLabel>
#include <QLineEdit>
#include <QVBoxLayout>

#include <WinSock2.h>
#include <WS2tcpip.h>
#include <iphlpapi.h>

#include "core/configuration.hpp"
#include "translator.hpp"

struct NetworkAdapter
{
    QString description;
    QString ipv4;
    QString ipv6;
};

using pt::ConnectionPreferencesPage;

ConnectionPreferencesPage::ConnectionPreferencesPage()
    : m_ui(new Ui::ConnectionPreferencesPage())
{
    m_ui->setupUi(this);

    // translate
    m_ui->listenInterfaceGroup->setTitle(i18n("listen_interface"));
    m_ui->networkAdapterLabel->setText(i18n("network_adapter"));
    m_ui->portLabel->setText(i18n("port"));
    m_ui->encryptionGroup->setTitle(i18n("encryption"));
    m_ui->encryptIncoming->setText(i18n("require_encryption_incoming"));
    m_ui->encryptOutgoing->setText(i18n("require_encryption_outgoing"));
    m_ui->privacyGroup->setTitle(i18n("privacy"));
    m_ui->enableDht->setText(i18n("enable_dht"));
    m_ui->enableLsd->setText(i18n("enable_lsd"));
    m_ui->enablePex->setText(i18n("enable_pex"));

    setupNetworkAdapters();
}

ConnectionPreferencesPage::~ConnectionPreferencesPage()
{
    delete m_ui;
}

void ConnectionPreferencesPage::loadConfig(std::shared_ptr<pt::Configuration> cfg)
{
    QString iface = QString::fromStdString(cfg->getString("listen_interface"));

    for (int i = 0; i < m_ui->listenInterfaces->count(); i++)
    {
        auto adapter = static_cast<NetworkAdapter*>(m_ui->listenInterfaces->itemData(i).value<void*>());

        if (adapter->ipv4 == iface || adapter->ipv6 == iface)
        {
            m_ui->listenInterfaces->setCurrentIndex(i);
        }
        else if (iface.contains("|"))
        {
            QStringList ifaces = iface.split("|");

            if (adapter->ipv4 == ifaces[0] || adapter->ipv6 == ifaces[1])
            {
                m_ui->listenInterfaces->setCurrentIndex(i);
            }
        }
    }

    m_ui->port->setValue(cfg->getInt("listen_port"));
    m_ui->encryptIncoming->setChecked(cfg->getBool("require_incoming_encryption"));
    m_ui->encryptOutgoing->setChecked(cfg->getBool("require_outgoing_encryption"));
    m_ui->enableDht->setChecked(cfg->getBool("enable_dht"));
    m_ui->enableLsd->setChecked(cfg->getBool("enable_lsd"));
    m_ui->enablePex->setChecked(cfg->getBool("enable_pex"));
}

void ConnectionPreferencesPage::saveConfig(std::shared_ptr<pt::Configuration> cfg)
{
    auto adapterIndex = m_ui->listenInterfaces->currentIndex();
    auto adapterData = m_ui->listenInterfaces->itemData(adapterIndex);
    auto adapter = static_cast<NetworkAdapter*>(adapterData.value<void*>());

    QStringList iface;
    iface << adapter->ipv4;

    if (!adapter->ipv6.isEmpty())
    {
        iface << adapter->ipv6;
    }

    cfg->setString("listen_interface", iface.join("|").toStdString());
    cfg->setInt("listen_port", m_ui->port->value());
    cfg->setBool("require_incoming_encryption", m_ui->encryptIncoming->checkState() == Qt::Checked);
    cfg->setBool("require_outgoing_encryption", m_ui->encryptOutgoing->checkState() == Qt::Checked);
    cfg->setBool("enable_dht", m_ui->enableDht->checkState() == Qt::Checked);
    cfg->setBool("enable_lsd", m_ui->enableLsd->checkState() == Qt::Checked);
    cfg->setBool("enable_pex", m_ui->enablePex->checkState() == Qt::Checked);
}

void ConnectionPreferencesPage::setupNetworkAdapters()
{
    // Add the <any> adapter
    auto any = new NetworkAdapter();
    any->description = "<any>";
    any->ipv4 = "0.0.0.0";
    any->ipv6 = "::";

    m_ui->listenInterfaces->addItem(
        any->description,
        QVariant::fromValue(static_cast<void*>(any)));

    ULONG bufferSize = 15000;
    IP_ADAPTER_ADDRESSES* pAddresses = new IP_ADAPTER_ADDRESSES[bufferSize];

    ULONG ret = GetAdaptersAddresses(
        AF_UNSPEC,
        GAA_FLAG_INCLUDE_PREFIX,
        NULL,
        pAddresses,
        &bufferSize);

    if (ret == ERROR_SUCCESS)
    {
        for (PIP_ADAPTER_ADDRESSES pCurrAddresses = pAddresses; pCurrAddresses != nullptr; pCurrAddresses = pCurrAddresses->Next)

        if (pCurrAddresses->IfType != IF_TYPE_SOFTWARE_LOOPBACK)
        {
            auto adapter = new NetworkAdapter();
            adapter->description = QString::fromStdWString(pCurrAddresses->FriendlyName);

            IP_ADAPTER_UNICAST_ADDRESS* pu = pCurrAddresses->FirstUnicastAddress;

            while (pu)
            {
                switch (pu->Address.lpSockaddr->sa_family)
                {
                case AF_INET:
                {
                    char buf[INET_ADDRSTRLEN] = {};
                    sockaddr_in *si = reinterpret_cast<sockaddr_in*>(pu->Address.lpSockaddr);
                    inet_ntop(AF_INET, &(si->sin_addr), buf, sizeof(buf));
                    adapter->ipv4 = buf;
                    break;
                }
                case AF_INET6:
                {
                    char buf[INET6_ADDRSTRLEN] = {};
                    sockaddr_in6 *si = reinterpret_cast<sockaddr_in6*>(pu->Address.lpSockaddr);
                    inet_ntop(AF_INET6, &(si->sin6_addr), buf, sizeof(buf));
                    adapter->ipv6 = buf;
                    break;
                }
                }

                pu = pu->Next;
            }

            m_ui->listenInterfaces->addItem(
                adapter->description,
                QVariant::fromValue(static_cast<void*>(adapter)));
        }
    }

    delete[] pAddresses;
}

