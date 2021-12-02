#pragma once
#include "Types.h"
#include <windows.h>
#include "src/database/db_bind.h"

{%- macro gen_procedures() -%} {% include 'Procedure.h' %} {% endmacro %}

namespace SP
{
	{{gen_procedures() | indent}}
};