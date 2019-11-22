
function main()
    print("Hello world!")

    for key,value in pairs(cit)
    do
    print("Interface: ", key)
    end

    for key,value in pairs(cpt)
    do
    print("Frame: ", key)
    end

    -- write(cit['do'], true)
    -- print("aaa")
    -- write(cit['do'], false)
    -- print("bbb")
    -- write(cit.ad, 100)
    -- print("ccc")
    -- print("MaxStep: ", maxStepTick())
    -- print("ddd")
end

main()