local SEPARATOR = "________________"
SEPARATOR = SEPARATOR .. SEPARATOR .. SEPARATOR

function build_scheme(command)
    local str = command.name .. " "
    for i,arg in ipairs(command.args) do
        if arg.optional then
            str = str .. "[" .. arg.name .. "] "
        else
            str = str .. "<" .. arg.name .. "> "
        end
    end
    return str
end

console.add_command(
    "clear",
    "Clears the console",
    function()
        local document = Document.new("core:console")
        document.log.text = ""
    end
)

console.add_command(
    "help name:str=''",
    "Show help infomation for the specified command",
    function(args, kwargs)
        local name = args[1]
        if #name == 0 then

            local commands = console.get_commands_list()
            table.sort(commands)
            local str = "Available commands:"

            for i,k in ipairs(commands) do
                str = str .. "\n  " .. build_scheme(console.get_command_info(k))
            end

            return str .. "\nuse 'help <command>'"

        end

        local command = console.get_command_info(name)

        if command == nil then
            return string.format("command %q not found", name)
        end

        local where = ":"
        local str = SEPARATOR .. "\n" .. command.description .. "\n" .. name .. " "

        for _, arg in ipairs(command.args) do
            where = where .. "\n  " .. arg.name .. " - " .. arg.type

            if arg.optional then
                str = str .. "[" .. arg.name .. "] "
                where = where .. " (optional)"
            else
                str = str .. "<" .. arg.name .. "> "
            end
        end

        if #command.args > 0 then
            str = str .. "\nwhere" .. where
        end

        return str .. "\n" .. SEPARATOR
    end
)

local function FormattedTime(seconds, format ) -- from gmod
    if not seconds then seconds = 0 end

    local hours = math.floor(seconds / 3600)
    local minutes = math.floor((seconds / 60) % 60)
    local millisecs = (seconds - math.floor(seconds)) * 100
    seconds = math.floor(seconds % 60)

    if format then
        return string.format(format, minutes, seconds, millisecs)
    else
        return {h = hours, m = minutes, s = seconds, ms = millisecs}
    end
end

console.add_command(
    "time.uptime",
    "Get time elapsed since the engine started",
    function()
        local uptime = time.uptime()
        local formatted_uptime = ""

        local t = FormattedTime(uptime)

        formatted_uptime = t.h .. "h " .. t.m .. "m " .. t.s .. "s"

        return formatted_uptime .. " (" .. uptime .. "s)"
    end
)

console.add_command(
    "tp entity:sel=$entity.id x:num~pos.x y:num~pos.y z:num~pos.z",
    "Teleport entity",
    function(args, kwargs)
        local eid, x, y, z = unpack(args)
        local entity = entities.get(eid)
        if entity then
            entity.transform:set_pos({x, y, z})
        end
    end
)
console.add_command(
    "echo value:str",
    "Print value to the console",
    function(args, kwargs)
        return args[1]
    end
)
console.add_command(
    "time.set value:num",
    "Set day time [0..1] where 0 is midnight, 0.5 is noon",
    function(args, kwargs)
        world.set_day_time(args[1])
        return "Time set to " .. args[1]
    end
)
console.add_command(
    "blocks.fill id:str x:num~pos.x y:num~pos.y z:num~pos.z w:int h:int d:int",
    "Fill specified zone with blocks",
    function(args, kwargs)
        local name, x, y, z, w, h, d = unpack(args)
        local id = block.index(name)
        for ly = 0, h - 1 do
            for lz = 0, d - 1 do
                for lx = 0, w - 1 do
                    block.set(x + lx, y + ly, z + lz, id)
                end
            end
        end
        return tostring(w * h * d) .. " blocks set"
    end
)

console.add_command(
    "player.respawn player:sel=$obj.id",
    "Respawn player entity",
    function(args, kwargs)
        local eid = entities.spawn("base:player", {player.get_pos(args[1])}):get_uid()
        player.set_entity(args[1], eid)
        return "spawned new player entity #" .. tostring(eid)
    end
)

console.add_command(
    "entity.despawn entity:sel=$entity.selected",
    "Despawn entity",
    function(args, kwargs)
        local eid = args[1]
        local entity = entities.get(eid)
        if entity ~= nil then
            entity:despawn()
            return "despawned entity #" .. tostring(eid)
        end
    end
)
