
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

function test_write_read()
    --write
    write(cit['do'], true)
    delay(100)
    print("writed do 1: ", true)
    print("read di value1: ", read(cit.di))

    write(cit['do'], false)
    delay(100)
    print("writed do 2: ", false)
    print("read di value2: ", read(cit.di))

    write(cit.ad, 100)
    delay(100)
    print("wirted ad: ", 100)
    print("read da value: ", read(cit.da))

    --sendBuffer
    local str = string.format("strdemo%dstrdeom", 12345)
    sendBuffer(cit.serial, str);
    print("sendBuffer serial 1: ", str)
    sendBuffer(cit.serial, str..str, {a=true});
    print("sendBuffer serial 2: ", str..str)

    local v, o = recvBuffer(cit.serial)
    print("recv serial buffer1:  v = ", v, "  o = ", o)
    v, o = recvBuffer(cit.serial)
    print("recv serial buffer2:  v = ", v, "  o = ", o)
    print("recv serial buffer3:  ", recvBuffer(cit.serial))

end


function test_send()
    --send
    local gps_data = { ["经度信息"]=99.888, ["纬度信息"]=777.678}
    send(cit['232'], cpt.pr_GPS, gps_data, {a=true})
end

function test_recv()
    local v, o = recv(cit['232'], cpt.pr_GPS, 3000);
    print("recv prot v:", v,  "o:", o)
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

function delay_write1()
    write(cit['do'], false)
end

function delay_write2()
    write(cit['do'], true)
    print("delay_write2")
end

function test_assertchange()
    print("")
    write(cit['do'], false)
    delay(500)
    timer.timeout(1000, delay_write2)
    assert.changeHeigh(cit.di, 5000, "not changeHeight")

    timer.timeout(900, delay_write1)
    assert.changeLow(cit.di, 1200, "not changeLow")
    print("assert change test passed")
    
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
    local gps_data = { ["经度信息"]=99.888, ["纬度信息"]=777 }
    send(cit['232'], cpt.pr_GPS, gps_data, {a=true})
    recvAsync(cit['232'], cpt.pr_GPS, 3000, afterRecvProt2)
    local gps_data = { ["经度信息"]=99.888, ["纬度信息"]=777 }
    send(cit['232'], cpt.pr_GPS, gps_data, {a=true})
end

function test_async()
    recvAsync(cit['232'], cpt.pr_GPS, 3000, afterRecvProt)
    local gps_data = { ["经度信息"]=99.888, ["纬度信息"]=777 }
    send(cit['232'], cpt.pr_GPS, gps_data, {a=true})
end

function test_waityesno()
    print("wait yes or no??")
    local is_yes = waitYesNo("yes or no??")
    print("result is: ", is_yes)
end

function test_timer()
    print("--on timeout--")
    local gps_data = { ["经度信息"]=99.888, ["纬度信息"]=777 }
    send(cit['232'], cpt.pr_GPS, gps_data, {a=true})
    delay(263)
    test_async()
end


function main()
    print("\n\nStart run case:")
    test_print()
    test_write_read()
    test_send()
    test_delay()
    test_recv()
    --test_assert()
    test_assertchange()
    test_async()
    timer.timeout(1000, test_timer)
    local t = timer.interval(100, 2000, test_timer)
    test_other()
    delay(1000)
    timer.stop(t)
    delay(2000)
    timer.timeout(3000, exit)
    test_waityesno()
end

main()

--print("\n\nStart run case:")
--test_print()
