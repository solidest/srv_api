
require "string"

function test_print()
    -- print
    for key,value in pairs(cit)
    do
    print("Interface: ", key)
    end

    for key,value in pairs(cpt)
    do
    print("Frame: ", key)
    end
end

function test_write()
    --write
    write(cit['do'], true)
    print("writed do 1")
    write(cit['do'], false)
    print("writed do 2")
    write(cit.ad, 100)
    print("wirted ad")
end

function test_sendBuffer()
    --sendBuffer
    local str = string.format("test %d", 100)
    sendBuffer(cit.serial, str);
    print("sendBuffer serial")

end

function test_send()
    --send
    local gps_data = { ["经度信息"]=99.888, ["纬度信息"]=777.678}
    send(cit['232'], cpt.pr_GPS, gps_data, {a=true})
end

function test_recv()
    local v, o = recv(cit['232'], cpt.pr_GPS, 3000);
    print("recv prot v: ", v["经度信息"], ", ", v["纬度信息"], "  o: ", o)
end

function test_delay()
    --delay
    print("before delay: ", now())
    delay(500);
    print("after delay: ", now())
end

function test_assert()
    --assert
    assert.ok(true)
    assert.ok(false, "eeeeeee")
end

function test_read()
    --recv
    print("read di value1: ", read(cit.di))
    print("read di value2: ", read(cit.di))
    print("read da value: ", read(cit.da))
    v, o = recvBuffer(cit.serial)
    print("recv serial buffer1:  string:", v, "  option:", o)
    print("recv serial buffer2:  ", recvBuffer(cit.serial))
end

function test_assertchange()
    write(cit['do'], true)
    write(cit['do'], false)
    assert.changeHeigh(cit.di, 3000)
    write(cit['do'], false)
    write(cit['do'], true)
    assert.changeHeigh(cit.di, 3000, "change ok")
    assert.changeHeigh(cit.di, 3000, "change bad")
end
     

function test_other()
    print("MaxStep: ", maxStepTick())
end

function afterRecvProt2(value, option)
    print("Async recved2: v=", value, "  o=", option)
    delay(500)
    write(cit['do'], true)
    local b = read(cit.di)
    print("read in async2:", b)
    
end

function afterRecvProt(value, option)
    print("Async recved: v=", value, "  o=", option)
    delay(500)
    write(cit['do'], true)
    local b = read(cit.di)
    print("read in async:", b)
    recvAsync(cit['232'], cpt.pr_GPS, 3000, afterRecvProt2)
    local gps_data = { ["经度信息"]=99.888, ["纬度信息"]=777 }
    send(cit['232'], cpt.pr_GPS, gps_data, {a=true})
end

function test_async()
    recvAsync(cit['232'], cpt.pr_GPS, 3000, afterRecvProt)
    local gps_data = { ["经度信息"]=99.888, ["纬度信息"]=777 }
    send(cit['232'], cpt.pr_GPS, gps_data, {a=true})
end


function main()
    print("\n\nStart run case:")
    test_print()
    test_write()
    test_sendBuffer()
    test_send()
    test_delay()
    test_read()
    test_recv()
    --test_assert()
    --test_assertchange()
    test_async()
    test_other()
end


main()
--print("\n\nStart run case:")
--test_print()
