#include "api_lua.hpp"

#include "logic/scripting/scripting_hud.hpp"
#include "graphics/render/WorldRenderer.hpp"
#include "graphics/render/TextsRenderer.hpp"
#include "graphics/render/TextNote.hpp"
#include "engine.hpp"

using namespace scripting;

static int l_show(lua::State* L) {
    auto position = lua::tovec3(L, 1);
    auto text = lua::require_wstring(L, 2);
    auto preset = lua::tovalue(L, 3);
    auto extension = lua::tovalue(L, 4);
    
    NotePreset notePreset {};
    notePreset.deserialize(preset);
    if (extension != nullptr) {
        notePreset.deserialize(extension);
    }
    auto note = std::make_unique<TextNote>(text, notePreset, position);
    return lua::pushinteger(L, renderer->texts->add(std::move(note)));
}

static int l_hide(lua::State* L) {
    renderer->texts->remove(lua::touinteger(L, 1));
    return 0;
}

static int l_get_text(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        return lua::pushwstring(L, note->getText());
    }
    return 0;
}

static int l_set_text(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        note->setText(lua::require_wstring(L, 2));
    }
    return 0;
}

static int l_get_pos(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        return lua::pushvec(L, note->getPosition());
    }
    return 0;
}
static int l_set_pos(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        note->setPosition(lua::tovec3(L, 2));
    }
    return 0;
}

static int l_get_axis_x(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        return lua::pushvec(L, note->getAxisX());
    }
    return 0;
}
static int l_set_axis_x(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        note->setAxisX(lua::tovec3(L, 2));
    }
    return 0;
}


static int l_get_axis_y(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        return lua::pushvec(L, note->getAxisY());
    }
    return 0;
}
static int l_set_axis_y(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        note->setAxisY(lua::tovec3(L, 2));
    }
    return 0;
}

static int l_update_settings(lua::State* L) {
    if (auto note = renderer->texts->get(lua::tointeger(L, 1))) {
        note->updatePreset(lua::tovalue(L, 2));
    }
    return 0;
}

const luaL_Reg text3dlib[] = {
    {"show", lua::wrap<l_show>},
    {"hide", lua::wrap<l_hide>},
    {"get_text", lua::wrap<l_get_text>},
    {"set_text", lua::wrap<l_set_text>},
    {"get_pos", lua::wrap<l_get_pos>},
    {"set_pos", lua::wrap<l_set_pos>},
    {"get_axis_x", lua::wrap<l_get_axis_x>},
    {"set_axis_x", lua::wrap<l_set_axis_x>},
    {"get_axis_y", lua::wrap<l_get_axis_y>},
    {"set_axis_y", lua::wrap<l_set_axis_y>},
    {"update_settings", lua::wrap<l_update_settings>},
    {NULL, NULL}
};
