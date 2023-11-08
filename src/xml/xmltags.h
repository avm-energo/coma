#pragma once

/// \brief Namespace for storaging names of base XML tags.
namespace tags
{
// Main tags
constexpr auto module = "module";
constexpr auto res = "resources";
constexpr auto sigs = "signals";
constexpr auto tabs = "section-tabs";
constexpr auto sections = "sections";
constexpr auto section = "section";
constexpr auto sgroup = "sgroup";
constexpr auto alarms = "alarms";
constexpr auto crit = "critical";
constexpr auto warn = "warning";
constexpr auto info = "info";
constexpr auto journals = "journals";
constexpr auto work = "work";
constexpr auto meas = "meas";
constexpr auto system = "system";
constexpr auto modbus = "modbus";
constexpr auto protocom = "protocom";
constexpr auto iec = "iec60870";
constexpr auto config = "config";
constexpr auto conf_tabs = "config-tabs";
constexpr auto hidden = "hidden";

// Specified tags
constexpr auto sig = "signal";
constexpr auto tab = "tab";
constexpr auto mwidget = "mwidget";
constexpr auto item = "item";
constexpr auto group = "group";
constexpr auto record = "record";
constexpr auto highlights = "highlights";

// Common tags
constexpr auto mtypeb = "mtypeb";
constexpr auto mtypem = "mtypem";
constexpr auto version = "version";
constexpr auto desc = "desc";
constexpr auto prefix = "prefix";
constexpr auto background = "background";
constexpr auto title = "title";
constexpr auto id = "id";
constexpr auto addr = "addr";
constexpr auto start_addr = "start-addr";
constexpr auto src_addr = "src-addr";
constexpr auto dst_addr = "dst-addr";
constexpr auto count = "count";
constexpr auto header = "header";
constexpr auto tooltip = "toolTip";
constexpr auto str_array = "string-array";
constexpr auto string = "string";
constexpr auto sig_id = "signal-id";
constexpr auto sig_group = "sig-group";
constexpr auto sig_type = "sig-type";
constexpr auto reg_type = "reg-type";
constexpr auto trans_type = "trans-type";
constexpr auto type = "type";
constexpr auto block = "block";
constexpr auto def_val = "defaultValue";
constexpr auto visibility = "visibility";
constexpr auto s2files = "s2files";
constexpr auto class_ = "class";
constexpr auto min = "min";
constexpr auto max = "max";
constexpr auto decimals = "decimals";
constexpr auto field = "field";
constexpr auto data = "data";
constexpr auto bitfield = "bitfield";
constexpr auto parent = "parent";
constexpr auto name = "name";
constexpr auto widget = "widget";
constexpr auto view = "view";
constexpr auto index = "index";
}
