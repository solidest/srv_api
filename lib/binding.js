const addon = require('../build/Release/srv_api-native');

function SrvApi(name) {
    this.greet = function(str) {
        return _addonInstance.greet(str);
    }

    var _addonInstance = new addon.SrvApi(name);
}

module.exports = SrvApi;
