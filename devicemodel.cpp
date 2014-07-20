#include "devicemodel.h"
#include <QtCore/QtDebug>

#ifdef Q_OS_WIN32
#include <d3d9.h>
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }
#endif

DeviceModel::DeviceModel(QObject *parent) :
    QAbstractTableModel(parent)
{
}

void DeviceModel::load()
{
#ifdef Q_OS_WIN32
    HRESULT hr;
    IDirect3D9 *d3d = Direct3DCreate9(D3D_SDK_VERSION);
    D3DADAPTER_IDENTIFIER9 deviceInfo;

    if (!d3d) {
        emit(loadFailed());
        return;
    }

    UINT adapterCount = d3d->GetAdapterCount();
    for (UINT iAdapter = 0; iAdapter < adapterCount; iAdapter++ )
    {
        GraphicsDevice dev;

        hr = d3d->GetAdapterIdentifier(iAdapter, 0, &deviceInfo);
        if (!SUCCEEDED(hr)) {
            SAFE_RELEASE(d3d);
            emit(loadFailed());
            return;
        }

        dev.name = QString(deviceInfo.Description);
        dev.driver = QString(deviceInfo.Driver);
        dev.deviceId = deviceInfo.DeviceId;
        dev.vendorId = deviceInfo.VendorId;

        IDirect3DDevice9* d3dDevice = nullptr;
        HWND hWnd = GetDesktopWindow();

        D3DPRESENT_PARAMETERS pp;
        ZeroMemory( &pp, sizeof( D3DPRESENT_PARAMETERS ) );
        pp.BackBufferWidth = 800;
        pp.BackBufferHeight = 600;
        pp.BackBufferFormat = D3DFMT_R5G6B5;
        pp.BackBufferCount = 1;
        pp.MultiSampleType = D3DMULTISAMPLE_NONE;
        pp.MultiSampleQuality = 0;
        pp.SwapEffect = D3DSWAPEFFECT_DISCARD;
        pp.hDeviceWindow = hWnd;
        pp.Windowed = TRUE;

        pp.EnableAutoDepthStencil = FALSE;
        pp.Flags = 0;
        pp.FullScreen_RefreshRateInHz = 0;
        pp.PresentationInterval = D3DPRESENT_INTERVAL_DEFAULT;

        hr = d3d->CreateDevice( iAdapter, D3DDEVTYPE_HAL, hWnd,
                                  D3DCREATE_SOFTWARE_VERTEXPROCESSING, &pp, &d3dDevice );
        if (SUCCEEDED(hr)) {
            UINT availableTextureMem  = d3dDevice->GetAvailableTextureMem();
            dev.memory = availableTextureMem;
            SAFE_RELEASE( d3dDevice );

            beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count());
            m_devices.append(dev);
            endInsertRows();
        }

        SAFE_RELEASE(d3dDevice);
    }

    SAFE_RELEASE(d3d);
    emit(loaded());
#else
    // Insert bogus devices for testing
    GraphicsDevice dev1;
    dev1.name = "NVidia Debugging Device 1";
    dev1.driver = "nvd3dum.dll";
    dev1.vendorId = 0x10b4;
    dev1.deviceId = 0xFFFF;
    dev1.memory = 1024*1048576;

    GraphicsDevice dev2;
    dev2.name = "NVIDIA GeForce GTX 770";
    dev2.driver = "nvd3dum.dll";
    dev2.vendorId = 0x10de;
    dev2.deviceId = 0x1184;
    dev2.memory = (quint64)4096 * (quint64)1048576;

    GraphicsDevice dev3;
    dev3.name = "AMD Radeon HD 6800 Series";
    dev3.driver = "aticfx32.dll";
    dev3.vendorId = 0x1002;
    dev3.deviceId = 0x6738;
    dev3.memory = 64*1048576;

    GraphicsDevice dev4;
    dev4.name = "Intel(R) HD Graphics";
    dev4.driver = "igdumdim32.dll";
    dev4.vendorId = 0x8086;
    dev4.deviceId = 0x0F31;
    dev4.memory = 32*1048576;

    beginInsertRows(QModelIndex(), 0, 3);
    m_devices.append(dev1);
    m_devices.append(dev2);
    m_devices.append(dev3);
    m_devices.append(dev4);
    endInsertRows();

#endif
}

QVariant DeviceModel::data(const QModelIndex &index, int role) const
{
    if (role != Qt::DisplayRole && role != Qt::ToolTipRole) {
        return QVariant();
    }

    GraphicsDevice dev = m_devices.at(index.row());
    switch(index.column()) {
    case 0:
        return dev.name;
    case 1:
        return dev.vendorId;
    case 2:
        return dev.deviceId;
    case 3:
        return dev.driver;
    case 4:
        return dev.memory;
    }
    return QVariant();
}

QVariant DeviceModel::headerData(int section, Qt::Orientation orientation, int role) const
{
    if (orientation != Qt::Horizontal || role != Qt::DisplayRole) {
        return QVariant();
    }

    switch(section) {
    case 0:
        return "Name";
    case 1:
        return "Vendor ID";
    case 2:
        return "Device ID";
    case 3:
        return "Driver name";
    case 4:
        return "Texture Memory (bytes)";
    }
    return QVariant();
}
