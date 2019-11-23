const {SrvApi} = require('../build/Release/srv_api-native');
const assert = require("assert");

assert(SrvApi, "The expected module is undefined");

function testBasic()
{
    let srv = new SrvApi("127.0.0.1", 1210);
    let s = srv.state();
    assert.ok(s.work_type=="idle" || s.work_type=="prepared");

    if(s.work_type=="idle") {
        srv.prepare(1, "/home/pi/srv_api/demo/interface", "/home/pi/srv_api/demo/protocol");
        s = srv.state();
        assert.ok(s.work_type=="prepared");        
    }


    let script = "/home/pi/srv_api/demo/case/case1.lua";
    srv.start(1, script);
    //s = srv.state();
    //assert.ok(s.work_type=="running");
    for(var t = Date.now(); Date.now() - t <= 1500;);

    // srv.pause(1, script);
    // s = srv.state();
    // assert.ok(s.work_type=="pause");

    // srv.continue(1, script);
    // s = srv.state();
    // assert.ok(s.work_type=="running");

    // srv.stop(1, script);
    // for(var t = Date.now();Date.now() - t <= 200;);
    // s = srv.state();
    // assert.ok(s.work_type=="prepared");

    i = Date.now()%2;
    s = srv.state();
    if(i==0 && s.work_type=="prepared") {
        srv.clear(1);
        s = srv.state();
        assert.ok(s.work_type=="idle");
    }
    
    for(;;) {
        s = srv.state();
        if(s.work_type=="running") {
            continue;
        } else {
            break;
        }
    }

    //srv.exit();
}

function testInvalidParams()
{
    const srv = new SrvApi();
}

function testLoop() {
    for(let i =0; i<1000; i++) {
        testBasic();
    }
}

testLoop();
//assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");
assert.throws(testInvalidParams, undefined, "testInvalidParams didn't throw");

console.log("Tests passed- everything looks OK!");