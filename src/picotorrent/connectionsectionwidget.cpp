#include "connectionsectionwidget.hpp"

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

#include "configuration.hpp"
#include "translator.hpp"

struct NetworkAdapter
{
    QString description;
    QString guid;
    QString ipv4;
    QString ipv6;
};

using pt::ConnectionSectionWidget;

ConnectionSectionWidget::ConnectionSectionWidget()
{
    createUi();
    setupNetworkAdapters();

    connect(
        m_listenInterfaces,
        QOverload<int>::of(&QComboBox::currentIndexChanged),
        this,
        &ConnectionSectionWidget::onListenInterfaceChanged);
}

void ConnectionSectionWidget::loadConfig(std::shared_ptr<pt::Configuration> cfg)
{
    QString iface = QString::fromStdString(cfg->getString("listen_interface"));

    for (int i = 0; i < m_listenInterfaces->count(); i++)
    {
        auto adapter = static_cast<NetworkAdapter*>(m_listenInterfaces->itemData(i).value<void*>());

        if (adapter->guid == iface)
        {
            m_listenInterfaces->setCurrentIndex(i);
        }
    }

    m_port->setText(QString::number(cfg->getInt("listen_port")));
    m_encryptIncoming->setChecked(cfg->getBool("require_incoming_encryption"));
    m_encryptOutgoing->setChecked(cfg->getBool("require_outgoing_encryption"));
    m_enableDht->setChecked(cfg->getBool("enable_dht"));
    m_enableLsd->setChecked(cfg->getBool("enable_lsd"));
    m_enablePex->setChecked(cfg->getBool("enable_pex"));
}

void ConnectionSectionWidget::saveConfig(std::shared_ptr<pt::Configuration> cfg)
{
    auto adapterIndex = m_listenInterfaces->currentIndex();
    auto adapterData = m_listenInterfaces->itemData(adapterIndex);
    auto adapter = static_cast<NetworkAdapter*>(adapterData.value<void*>());

    cfg->setString("listen_interface", adapter->guid.toStdString());
    cfg->setInt("listen_port", m_port->text().toInt());
    cfg->setBool("require_incoming_encryption", m_encryptIncoming->checkState() == Qt::Checked);
    cfg->setBool("require_outgoing_encryption", m_encryptOutgoing->checkState() == Qt::Checked);
    cfg->setBool("enable_dht", m_enableDht->checkState() == Qt::Checked);
    cfg->setBool("enable_lsd", m_enableLsd->checkState() == Qt::Checked);
    cfg->setBool("enable_pex", m_enablePex->checkState() == Qt::Checked);
}

void ConnectionSectionWidget::createUi()
{
    m_listenInterfaces = new QComboBox();
    m_port = new QLineEdit();
    m_ipv4 = new QLabel();
    m_ipv6 = new QLabel();
    m_encryptIncoming = new QCheckBox(i18n("require_encryption_incoming"));
    m_encryptOutgoing = new QCheckBox(i18n("require_encryption_outgoing"));
    m_enableDht = new QCheckBox(i18n("enable_dht"));
    m_enableLsd = new QCheckBox(i18n("enable_lsd"));
    m_enablePex = new QCheckBox(i18n("enable_pex"));

    m_port->setAlignment(Qt::AlignRight);
    m_port->setMaximumWidth(40);
    m_port->setValidator(new QIntValidator());

    auto hl = new QFrame(this);
    hl->setFrameShape(QFrame::HLine);
    hl->setFrameShadow(QFrame::Sunken);

    auto listenLayout = new QGridLayout();
    listenLayout->addWidget(new QLabel("Network adapter"), 0, 0);
    listenLayout->addWidget(m_listenInterfaces, 0, 1);
    listenLayout->addWidget(new QLabel(i18n("port")), 1, 0);
    listenLayout->addWidget(m_port, 1, 1);
    listenLayout->addWidget(hl, 2, 0, 1, 2);
    listenLayout->addWidget(new QLabel("IPv4 address"), 3, 0);
    listenLayout->addWidget(m_ipv4, 3, 1);
    listenLayout->addWidget(new QLabel("IPv6 address"), 4, 0);
    listenLayout->addWidget(m_ipv6, 4, 1);

    auto listenGroup = new QGroupBox(i18n("listen_interface"));
    listenGroup->setLayout(listenLayout);

    auto encryptionLayout = new QVBoxLayout();
    encryptionLayout->addWidget(m_encryptIncoming);
    encryptionLayout->addWidget(m_encryptOutgoing);

    auto encryptionGroup = new QGroupBox(i18n("encryption"));
    encryptionGroup->setLayout(encryptionLayout);

    auto privacyLayout = new QHBoxLayout();
    privacyLayout->addWidget(m_enableDht);
    privacyLayout->addWidget(m_enableLsd);
    privacyLayout->addWidget(m_enablePex);

    auto privacyGroup = new QGroupBox(i18n("privacy"));
    privacyGroup->setLayout(privacyLayout);

    auto layout = new QVBoxLayout(this);
    layout->addWidget(listenGroup);
    layout->addWidget(encryptionGroup);
    layout->addWidget(privacyGroup);
    layout->addStretch();
    layout->setMargin(0);

    this->setLayout(layout);
}

void ConnectionSectionWidget::onListenInterfaceChanged(int index)
{
    auto adapter = static_cast<NetworkAdapter*>(m_listenInterfaces->itemData(index).value<void*>());
    m_ipv4->setText(adapter->ipv4);
    m_ipv6->setText(adapter->ipv6);
}

void ConnectionSectionWidget::setupNetworkAdapters()
{
    // Add the <any> adapter
    auto any = new NetworkAdapter();
    any->description = "<any>";
    any->guid = "{any}";
    any->ipv4 = "0.0.0.0";
    any->ipv6 = "::";

    m_listenInterfaces->addItem(
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
        IP_ADAPTER_ADDRESSES* pCurrAddresses = pAddresses;

        while (pCurrAddresses)
        {
            if (pCurrAddresses->IfType != IF_TYPE_SOFTWARE_LOOPBACK)
            {
                auto adapter = new NetworkAdapter();

                char guidBuf[64];
                sprintf_s(guidBuf, 64, "{%08X-%04X-%04X-%02X%02X-%02X%02X%02X%02X%02X%02X}",
                    pCurrAddresses->NetworkGuid.Data1, pCurrAddresses->NetworkGuid.Data2, pCurrAddresses->NetworkGuid.Data3,
                    pCurrAddresses->NetworkGuid.Data4[0], pCurrAddresses->NetworkGuid.Data4[1],
                    pCurrAddresses->NetworkGuid.Data4[2], pCurrAddresses->NetworkGuid.Data4[3],
                    pCurrAddresses->NetworkGuid.Data4[4], pCurrAddresses->NetworkGuid.Data4[5],
                    pCurrAddresses->NetworkGuid.Data4[6], pCurrAddresses->NetworkGuid.Data4[7]);

                adapter->description = QString::fromStdWString(pCurrAddresses->Description);
                adapter->guid = guidBuf;

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

                m_listenInterfaces->addItem(
                    adapter->description,
                    QVariant::fromValue(static_cast<void*>(adapter)));
            }

            pCurrAddresses = pCurrAddresses->Next;
        }
    }

    delete[] pAddresses;
}
