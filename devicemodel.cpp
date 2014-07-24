#include "devicemodel.h"
#include <QtCore/QtDebug>

#ifdef Q_OS_WIN32
#include <d3d9.h>
#endif

#ifndef SAFE_RELEASE
#define SAFE_RELEASE(p)      { if (p) { (p)->Release(); (p)=nullptr; } }
#endif

uint qHash(GraphicsMode key, uint seed) {
    quint32 otherKey = (quint32)(
        (((key.width ^ key.refreshRate) << 16)
        | (key.height ^ key.refreshRate)) & 0xFFFFFFFF
    );
    return qHash(otherKey, seed);
}

bool GraphicsMode::operator==(const GraphicsMode& other) const
{
    return width == other.width && height == other.height && refreshRate == other.refreshRate;
}

bool GraphicsMode::operator<(const GraphicsMode& other) const
{
    return (width < other.width) ||
           (width == other.width && height < other.height) ||
           (width == other.width && height == other.height && refreshRate < other.refreshRate);
}

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

    const int formatCount = 6;
    D3DFORMAT modeFormats[formatCount] = {
        D3DFMT_A1R5G5B5,
        D3DFMT_A2R10G10B10,
        D3DFMT_A8R8G8B8,
        D3DFMT_R5G6B5,
        D3DFMT_X1R5G5B5,
        D3DFMT_X8R8G8B8
    };

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
        quint16 driverMajor = (deviceInfo.DriverVersion.HighPart >> 16);
        quint16 driverMinor = (deviceInfo.DriverVersion.HighPart & 0xFFFF);
        quint16 driverBuild = (deviceInfo.DriverVersion.LowPart >> 16);
        quint16 driverRevision = (deviceInfo.DriverVersion.LowPart & 0xFFFF);
        QString driverVersion = QString("%1.%2.%3.%4").arg(driverMajor).arg(driverMinor).arg(driverBuild).arg(driverRevision);
        dev.driver = QString(deviceInfo.Driver) + " " + driverVersion;
        dev.display = QString(deviceInfo.DeviceName);
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

            // TODO Fixup code - can't test this in Linux yet :)
            for (int iFormat = 0; iFormat < formatCount; ++iFormat) {
                D3DFORMAT format = modeFormats[iFormat];
                int modeCount = d3d->GetAdapterModeCount(iAdapter, format);

                for (int iMode = 0; iMode < modeCount; ++iMode) {
                    D3DDISPLAYMODE mode;
                    hr = d3d->EnumAdapterModes(iAdapter, format, iMode, &mode);
                    if (SUCCEEDED(hr)) {
                        GraphicsMode gMode;
                        gMode.width = mode.Width;
                        gMode.height = mode.Height;
                        gMode.refreshRate = mode.RefreshRate;
                        dev.modes.append(gMode);
                    }
                }
            }

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

    // First some modes
    GraphicsMode mode800_600_60;
    mode800_600_60.width = 800;
    mode800_600_60.height = 600;
    mode800_600_60.refreshRate = 60;

    GraphicsMode mode1024_768_50;
    mode1024_768_50.width = 1024;
    mode1024_768_50.height = 768;
    mode1024_768_50.refreshRate = 50;

    GraphicsMode mode1280_1024_75;
    mode1280_1024_75.width = 1280;
    mode1280_1024_75.height = 1024;
    mode1280_1024_75.refreshRate = 75;

    GraphicsMode mode1600_1200_59;
    mode1600_1200_59.width = 1600;
    mode1600_1200_59.height = 1200;
    mode1600_1200_59.refreshRate = 59;

    GraphicsMode mode1680_1050_60;
    mode1680_1050_60.width = 1680;
    mode1680_1050_60.height = 1050;
    mode1680_1050_60.refreshRate = 60;

    GraphicsMode mode1920_1080_59;
    mode1920_1080_59.width = 1920;
    mode1920_1080_59.height = 1080;
    mode1920_1080_59.refreshRate = 59;

    GraphicsMode mode1920_1080_60;
    mode1920_1080_60.width = 1920;
    mode1920_1080_60.height = 1080;
    mode1920_1080_60.refreshRate = 60;

    GraphicsMode mode1920_1200_60;
    mode1920_1200_60.width = 1920;
    mode1920_1200_60.height = 1200;
    mode1920_1200_60.refreshRate = 60;

    GraphicsMode mode2560_1440_60;
    mode2560_1440_60.width = 2560;
    mode2560_1440_60.height = 1440;
    mode2560_1440_60.refreshRate = 60;

    GraphicsDevice dev1;
    dev1.name = "NVidia Debugging Device 1";
    dev1.driver = "nvd3dum.dll 1.0";
    dev1.display = "\\\\.\\DISPLAY1";
    dev1.vendorId = 0x10b4;
    dev1.deviceId = 0xFFFF;
    dev1.memory = 1024*1048576;
    dev1.modes << mode800_600_60 << mode1024_768_50 << mode1600_1200_59 << mode1920_1080_59 << mode1920_1080_60;

    GraphicsDevice dev2;
    dev2.name = "NVIDIA GeForce GTX 770";
    dev2.driver = "nvd3dum.dll 39.48";
    dev2.display = "\\\\.\\DISPLAY2";
    dev2.vendorId = 0x10de;
    dev2.deviceId = 0x1184;
    dev2.memory = (quint64)4096 * (quint64)1048576;
    dev2.modes << mode800_600_60 << mode1024_768_50 << mode1600_1200_59 << mode1920_1200_60;

    GraphicsDevice dev3;
    dev3.name = "AMD Radeon HD 6800 Series";
    dev3.driver = "aticfx32.dll 14.04";
    dev3.display = "\\\\.\\DISPLAY3";
    dev3.vendorId = 0x1002;
    dev3.deviceId = 0x6738;
    dev3.memory = 64*1048576;
    dev3.modes << mode800_600_60 << mode1024_768_50 << mode1280_1024_75 << mode1600_1200_59;

    GraphicsDevice dev4;
    dev4.name = "Intel(R) HD Graphics";
    dev4.driver = "igdumdim32.dll 13.37";
    dev4.display = "\\\\.\\DISPLAY4";
    dev4.vendorId = 0x8086;
    dev4.deviceId = 0x0F31;
    dev4.memory = 32*1048576;
    dev4.modes << mode1680_1050_60 << mode1920_1080_59 << mode1920_1080_60 << mode1920_1200_60 << mode2560_1440_60;

    // Note: last parameter is "last row" (i.e. inclusive, hence +3 == 4-1)
    beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count() + 3);
    m_devices.append(dev1);
    m_devices.append(dev2);
    m_devices.append(dev3);
    m_devices.append(dev4);
    endInsertRows();

    emit(loaded());

#endif
}

QList<GraphicsMode> DeviceModel::allModes()
{
    QSet<GraphicsMode> modes;
    foreach(const GraphicsDevice &dev, m_devices) {
        modes.unite(dev.modes.toSet());
    }

    QList<GraphicsMode> result = modes.toList();
    std::sort(result.begin(), result.end());

    return result;
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
        return dev.display;
    case 4:
        return dev.driver;
    case 5:
        return dev.memory;
    case 6: {
        QStringList modes;
        foreach(const GraphicsMode &mode, dev.modes) {
            modes << QString("%1x%2@%3").arg(mode.width).arg(mode.height).arg(mode.refreshRate);
        }
        return modes.join(";");
    }
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
        return tr("Name");
    case 1:
        return tr("Vendor ID");
    case 2:
        return tr("Device ID");
    case 3:
        return tr("Display");
    case 4:
        return tr("Driver name");
    case 5:
        return tr("Texture Memory (bytes)");
    case 6:
        return tr("Available Modes");
    }
    return QVariant();
}
