
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

    write(cit['do'], true)
    print("writed do 1")
    write(cit['do'], false)
    print("writed do 2")
    write(cit.ad, 100)
    print("wirted ad")
    print("MaxStep: ", maxStepTick())
    --assert.ok(false, "error1")
end

main()