
--print 测试
function test_print()
	print("<<begin test print>>")
	print("Hello World!")
	data = {a=100, b='"""%^&*(dljk%s'}
	print(data)
	print("<<end test print>>\n")
end

--delay 测试
function test_delay()
	print("<<begin test delay>>")
	delay(200)
	print("after delay 200ms")
	delay(1000)
	print("after delay 1s")
	print("<<end test delay>>\n")
end

--now 测试
function test_now()
	print("<<begin test now>>")
	print("now: ", now())
	delay(550)
	print("now: ", now())
	print("<<end test now>>\n")
end

--write read 测试
function test_write_read()
	print("<<begin test write & read>>")

	write(cit.digital_out, true)
	delay(300)
	local b = read(cit.digital_in)
	assert.ok(b, "数字接口write read失败")

	write(cit.analog_out, 1821)
	delay(300)
	local i = read(cit.analog_in)
	assert.ok(i==1821, "模拟接口write read失败")

	print("write read test passed")
	print("<<end test write & read>>\n")
end

--recv send测试
function test_send_recv()
	print("<<begin test send & recv>>")

	local data = {}
	data.point = {x=34, y=567}
	send(cit.istream, cpt.coordinate, data)
	local recv_data = recv(cit.istream, cpt.coordinate, 500)
	print("recved data1: ", recv_data)
	assert.ok(recv_data.point.x==34 and recv_data.point.y==567)
	recv_data = recv(cit.istream, cpt.coordinate, 500)
	print("recved data2: ", recv_data)
	assert.ok(not recv_data)
	print("<<end test send & recv>>\n")
end

--recvAsync 测试
function test_recvAsync()

	--call back for recvAsync
	function after_recved(recv_data, option)
		print("recved data: ", recv_data)
		print("recved option: ", option)
		assert.ok(recv_data.point.x==34 and recv_data.point.y==567)
		assert.ok(not option.is_ok)
		print("<<end test recvAsync>>\n")
	end

	print("<<begin test recvAsync>>")
	local data = {}
	data.point = {x=34, y=567}
	send(cit.istream, cpt.coordinate, data, {is_ok=false})
	recvAsync(cit.istream, cpt.coordinate, 500, after_recved)
	delay(500)
end

--sendBuffer recvBuffer 测试
function test_sendBuffer_recvBuffer()
	print("<<begin test sendBuffer & recvBuffer>>")
	local str = "abc%%%ddd"
	sendBuffer(cit.istream, str)
	delay(300)
	local recved_str = recvBuffer(cit.istream)
	print("recved buffer: ", recved_str)
	assert.ok(str=="abc%%%ddd")
	print("<<begin test sendBuffer & recvBuffer>>")
end

--reset 测试
function test_reset()
	print("<<begin test reset>>")
	local str = "fffffddddfff"
	delay(100)
	sendBuffer(cit.istream, str)
	reset(cit.istream)
	delay(300)
	local recved_str = recvBuffer(cit.istream)
	print("after reset recved: ", recved_str)
	--TODO 需要移动读取指针位置
	--assert.ok(not recved_str)
	print("<<end test reset>>\n")
end

--assert.change timer.timeout 测试
function test_assert_timeout()

	function delay_write(b)
		write(cit.digital_out, b)
	end

	print("<<begin test assert.change>>")
	
	write(cit.digital_out, true)
	delay(300)

	timer.timeout(500, delay_write, false)
	assert.changeLow(cit.digital_in, 600, "断言拉低失败")

	timer.timeout(500, delay_write, true)
	assert.changeHigh(cit.digital_in, 600, "断言拉高失败")

	print("<<end test assert.change>>\n")
end

--timer.interval timer_stop 测试
function test_interval_stop()
	local ii = 0
	function tick()
		ii = ii+1
		print("interval: ", ii)
	end

	print("<<begin test timer.interval & timer.stop>>")
	local t = timer.interval(0, 500, tick)
	delay(5000)
	timer.stop(t)
	print("<<end test timer.interval & timer.stop>>\n")

end

--waitYesNo exit 测试
function test_waitYesNo_exit()
	print("<<begin test waitYesNo & exit>>")
	local yes = waitYesNo("即将退出，是否延时3秒再退出？")
	print("result: ", yes)
	if yes then
		print("delay 3s")
		delay(3000)
	end
	exit()
	print("<<end test waitYesNo & exit>>\n")
end

test_print()
test_delay()
test_now()
test_write_read()
test_send_recv()
test_recvAsync()
test_sendBuffer_recvBuffer()
test_reset()
test_assert_timeout()
test_interval_stop()
test_waitYesNo_exit()
