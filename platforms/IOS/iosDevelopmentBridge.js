const DeviceLib = require("ios-device-lib");
const Path = require('path');

async function logApps(device) {
    console.log('\nApps: ')
    const result = dl.apps([device.deviceId])
    return result[0]
        .then(data => data.response)
        .then(response => {
            response.forEach(app => {
                console.log(app.CFBundleIdentifier)
            })

            return '\n\n\nlog zavershone\n\n\n'
        })
        .catch(err => console.error(err));
}

async function installApp(device, path) {
    return dl.install(path, [device.deviceId])[0]
        .then(resp => console.log(resp.response))
        .catch(err => console.error(err))
}

async function uninstallApp(device, app) {
    return dl.uninstall(app, [device.deviceId])[0]
        .then((resp) => console.log('\ndeleted\n', resp.response, '\n'))
        .catch(err => console.error('\ncant delete\n', err))
}

async function runCommand(device, command, arg) {
    switch(command){
        case 'log':
            await logApps(device)
            break;
        case 'install':
            const appPath = Path.resolve(__dirname, arg)
            await installApp(device, appPath)
            break;
        case 'uninstall':
            await uninstallApp(device, arg);
            break;
    }
}

async function handler(device) {
    const command = process.argv[2];
    const arg = process.argv[3];

    await runCommand(device, command, arg);

    process.exit()
}



function errorDeviceLost(device) {
    console.log("Device LOST!", device);
}

function deviceUpdate(device) {
    console.log("Device UPDATED!", device);
}

const dl = new DeviceLib.IOSDeviceLib(handler, deviceUpdate, errorDeviceLost);