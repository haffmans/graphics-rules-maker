/*
 * Graphics Rules Maker
 * Copyright (C) 2014 Wouter Haffmans <wouter@simply-life.net>
 *
 * This program is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * This program is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with this program.  If not, see <http://www.gnu.org/licenses/>.
 */

#include "devicemodel.h"
#include <QtCore/QtDebug>

#ifdef Q_OS_WIN32
#ifdef DXGI
#include <dxgi.h>
#endif
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

#ifdef Q_OS_WIN32
void DeviceModel::load()
{
    bool isLoaded = false;
#ifdef DXGI
    isLoaded = isLoaded || loadDxDgi();
#endif
    isLoaded = isLoaded || loadD3d9();

    if (isLoaded) {
        emit(loaded());
    }
    else {
        emit(loadFailed());
    }
}

bool DeviceModel::loadD3d9() {
    qDebug() << "Attempting D3D9...";
    HRESULT hr;
    IDirect3D9 *d3d = Direct3DCreate9(D3D_SDK_VERSION);
    D3DADAPTER_IDENTIFIER9 deviceInfo;

    if (!d3d) {
        qDebug() << "Failed creating D3D9 (SDK " << D3D_SDK_VERSION << ")";
        return false;
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
    qDebug() << "Adapter count:" << adapterCount;
    for (UINT iAdapter = 0; iAdapter < adapterCount; iAdapter++ )
    {
        GraphicsDevice dev;

        hr = d3d->GetAdapterIdentifier(iAdapter, 0, &deviceInfo);
        if (!SUCCEEDED(hr)) {
            SAFE_RELEASE(d3d);
            qDebug() << "Failed getting adapter info";
            return false;
        }

        dev.name = QString(deviceInfo.Description);
        qDebug() << "Got device" << dev.name;
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
            qDebug() << "Device created";
            UINT availableTextureMem  = d3dDevice->GetAvailableTextureMem();
            dev.memory = availableTextureMem;

            for (int iFormat = 0; iFormat < formatCount; ++iFormat) {
                D3DFORMAT format = modeFormats[iFormat];
                int modeCount = d3d->GetAdapterModeCount(iAdapter, format);

                qDebug() << "Mode count" << modeCount;
                for (int iMode = 0; iMode < modeCount; ++iMode) {
                    D3DDISPLAYMODE mode;
                    hr = d3d->EnumAdapterModes(iAdapter, format, iMode, &mode);
                    if (SUCCEEDED(hr)) {
                        GraphicsMode gMode;
                        gMode.width = mode.Width;
                        gMode.height = mode.Height;
                        gMode.refreshRate = mode.RefreshRate;
                        dev.modes.append(gMode);
                        qDebug() << "Got mode " << gMode.width << "x" << gMode.height << "@" << gMode.refreshRate;
                    }
                }
            }

            SAFE_RELEASE( d3dDevice );

            beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count());
            m_devices.append(dev);
            endInsertRows();
        }
        else {
            qDebug() << "Device not created";
        }

        SAFE_RELEASE(d3dDevice);
    }

    SAFE_RELEASE(d3d);
    qDebug() << "D3D9 Completed";
    return true;
}

#ifdef DXGI
bool DeviceModel::loadDxDgi()
{
    // Attempt to load dxgi.dll - this won't exist on Windows XP!
    HMODULE dxgi = LoadLibrary("dxgi.dll");

    if (dxgi == NULL) {
        qDebug() << "DXGI module not found...";
        return false;
    }

    // Locate CreateDXGIFactory function
    typedef HRESULT (WINAPI * CREATEDXGIFACTORY)(REFIID, void**);
    CREATEDXGIFACTORY createDxgiFactory = (CREATEDXGIFACTORY)GetProcAddress(dxgi, "CreateDXGIFactory");

    if (createDxgiFactory == NULL) {
        qDebug() << "DXGI factory function not found...";
        return false;
    }

    // And use it
    IDXGIFactory *factory;
    HRESULT hr = createDxgiFactory(__uuidof(IDXGIFactory), (void**)(&factory) );

    if (!SUCCEEDED(hr)) {
        qDebug() << "DXGI factory not created...";
        return false;
    }

    qDebug() << "DXGI Enumerating adapters";
    UINT i = 0;
    IDXGIAdapter *adapter;
    while(factory->EnumAdapters(i, &adapter) != DXGI_ERROR_NOT_FOUND) {
        DXGI_ADAPTER_DESC description;
        ZeroMemory(&description, sizeof(DXGI_ADAPTER_DESC));

        if (adapter->GetDesc(&description) == S_OK) {
            // Skip the "Microsoft Basic Render Driver" in Windows 8
            if (description.VendorId == 0x1414 && description.DeviceId == 0x8c) {
                SAFE_RELEASE(adapter);
                ++i;
                continue;
            }

            GraphicsDevice dev;

            // Get essential info
            dev.name = QString::fromWCharArray(description.Description);
            dev.deviceId = description.DeviceId;
            dev.vendorId = description.VendorId;
            dev.memory = description.DedicatedVideoMemory + description.DedicatedSystemMemory + description.SharedSystemMemory;
            qDebug() << "Found graphics card: " << qPrintable(dev.name) << "; Vendor " << description.VendorId << "; Device" << description.DeviceId;

            dev.driver = "WDDM Driver";

            // Get displays
            QStringList displays;
            UINT j = 0;
            IDXGIOutput *output;
            while(adapter->EnumOutputs(j, &output) != DXGI_ERROR_NOT_FOUND) {
                DXGI_OUTPUT_DESC outputDescription;

                if (output->GetDesc(&outputDescription) == S_OK) {
                    displays << QString::fromWCharArray(outputDescription.DeviceName, 32);
                }

                for (DXGI_FORMAT format = DXGI_FORMAT_UNKNOWN; format <= DXGI_FORMAT_B4G4R4A4_UNORM; format = DXGI_FORMAT(format + 1)) {
                    UINT modeCount;
                    output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &modeCount, NULL);
                    DXGI_MODE_DESC *modeDescriptions = new DXGI_MODE_DESC[modeCount];
                    output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, &modeCount, modeDescriptions);

                    for (int m = 0; m < modeCount; ++m) {
                        DXGI_MODE_DESC modeDescription = modeDescriptions[m];
                        output->GetDisplayModeList(format, DXGI_ENUM_MODES_INTERLACED, NULL, &modeDescription);

                        GraphicsMode gMode;
                        gMode.width = modeDescription.Width;
                        gMode.height = modeDescription.Height;
                        gMode.refreshRate = modeDescription.RefreshRate.Numerator / modeDescription.RefreshRate.Denominator;
                        dev.modes.append(gMode);
                    }
                }

                SAFE_RELEASE(output);
                j++;
            }

            // Make the set of modes unique again
            std::sort(dev.modes.begin(), dev.modes.end());
            QList<GraphicsMode>::Iterator modesEnd = std::unique(dev.modes.begin(), dev.modes.end());
            dev.modes.erase(modesEnd, dev.modes.end());

            if (!displays.isEmpty()) {
                dev.display = displays.join(", ");
            }
            else {
                dev.display = tr("[Disconnected]");
            }

            beginInsertRows(QModelIndex(), m_devices.count(), m_devices.count());
            m_devices.append(dev);
            endInsertRows();
        }
        SAFE_RELEASE(adapter);

        ++i;
    }

    SAFE_RELEASE(factory);

    FreeLibrary(dxgi);
    qDebug() << m_devices.count() << "DXGI devices loaded";
    return (m_devices.count() > 0);
}
#endif

#else
void DeviceModel::load()
{
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

}
#endif

QList<GraphicsMode> DeviceModel::allModes()
{
    QSet<GraphicsMode> modes;
    foreach(const GraphicsDevice &dev, m_devices) {
        modes.unite(dev.modes.toSet());
    }

    auto result = modes.values();
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

#include "moc_devicemodel.cpp"
