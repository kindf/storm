
LastPrintTs = os.time()
function Update()
    local now = os.time()
    if now >= LastPrintTs + 1 then
        LastPrintTs = now
        send("hello world. ts: "..LastPrintTs)
    end
end

function RequireEx(_mname)
    print(string.format("RequireEx = s", _mname))
    if package.loaded[_mname] then
        print(string.format("RequireEx module[ s] reload", _mname))
    end
    package.loaded[_mname] = nil
    require(_mname)
end
