const {SrvApi} = require('../build/Release/srv_api-native');
const assert = require("assert");

assert(SrvApi, "The expected module is undefined");

function testBasic()
{
    let srv = new SrvApi("127.0.0.1", 1210);
    let s = srv.state();
    assert.ok(s.work_type=="idle");

    srv.prepare(1, " ", " ");
    s = srv.state();
    assert.ok(s.work_type=="prepared");

    srv.start(1, " ");
    s = srv.state();
    assert.ok(s.work_type=="running");

    srv.pause(1, " ");
    s = srv.state();
    assert.ok(s.work_type=="pause");

    srv.continue(1, " ");
    s = srv.state();
    assert.ok(s.work_type=="running");

    srv.stop(1, " ");
    s = srv.state();
    assert.ok(s.work_type=="prepared");

    srv.clear(1);
    s = srv.state();
    assert.ok(s.work_type=="idle");

    srv.exit();
}

function testInvalidParams()
{
    const srv = new SrvApi();
}

//testBasic();
assert.doesNotThrow(testBasic, undefined, "testBasic threw an expection");
assert.throws(testInvalidParams, undefined, "testInvalidParams didn't throw");

console.log("Tests passed- everything looks OK!");