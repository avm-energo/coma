include(../common.pri)
include(../lib.pri)
TEMPLATE=lib
LIB=dummy

HEADERS += \
    detail/algorithm.hpp \
    detail/cstring.hpp \
    detail/entity_name.hpp \
    detail/hash.hpp \
    detail/language_features.hpp \
    detail/meta.hpp \
    detail/name_filters.hpp \
    detail/nlohmann_json.hpp \
    detail/preprocessor.hpp \
    detail/pretty_function.hpp \
    detailed_nameof.hpp \
    hash_literal.hpp \
    map.hpp \
    model.hpp \
    name.hpp \
    nameof.hpp \
    static_value.hpp \
    symbol.hpp \
    symbol_from_hash.hpp \
    tie.hpp \
    type_id.hpp \
    type_tag.hpp
	
#headers.files=$$HEADERS
#headers.path=$$PREFIX/include
#INSTALLS+=headers
