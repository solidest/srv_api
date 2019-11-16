const addon = require('../build/Release/srv_api-native');

function SrvApi(ip, port) {
    this.state = function() {
        return _addonInstance.state();
    }

    var _addonInstance = new addon.SrvApi(ip, port);
}

module.exports = SrvApi;
