
LastPrintTs = os.time()
function Update(pid)
    local now = os.time()
    if now >= LastPrintTs + 1 then
        print(string.format("lua update. ts:%i, pid:%i", now, pid or 1))
        LastPrintTs = now
    end
end

function Require_ex(_mname)
    print(string.format("require_ex = s", _mname))
    if package.loaded[_mname] then
        print(string.format("require_ex module[ s] reload", _mname))
    end
    package.loaded[_mname] = nil
    require(_mname)
end
