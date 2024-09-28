#ifndef _MIN_JSON_H_
#define _MIN_JSON_H_

#include "json_spirit_value.h"
#include "json_spirit_writer.h"
#include "json_spirit_reader.h"
#include "json_spirit_utils.h"

#pragma once

namespace sc
{
    // jgkim
	class Json
	{
	public:
		template< typename T >
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, T& Value)
		{
			return false;
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, LONG& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::int_type != i->second.type() )
					{
						return false;
					}
					else
					{
						int Temp = i->second.get_int();
						if ( Temp > LONG_MAX )
						{
							return false;
						}
						else
						{
							Value = static_cast< LONG >( Temp );
							return true;
						}
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, short& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::int_type != i->second.type() )
					{
						return false;
					}
					else
					{
						int Temp = i->second.get_int();
						if ( Temp > SHRT_MAX )
						{
							return false;
						}
						else
						{
							Value = static_cast< short >( Temp );
							return true;
						}
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, BYTE& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::int_type != i->second.type() )
					{
						return false;
					}
					else
					{
						int Temp = i->second.get_int();
						if ( Temp > UCHAR_MAX )
						{
							return false;
						}
						else
						{
							Value = static_cast< BYTE >( Temp );
							return true;
						}
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, json_spirit::mObject& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::obj_type != i->second.type() )
					{
						return false;
					}
					else
					{
						Value = i->second.get_obj();
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}
		}

        template<>
        static bool find_value(const json_spirit::mObject& obj, const std::string& name, json_spirit::mArray& jArray)
        {
            try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::array_type != i->second.type() )
					{
						return false;
					}
					else
					{
						jArray = i->second.get_array();
    					return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}
        }

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, std::string& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::str_type != i->second.type() )
					{
						return false;
					}
					else
					{
						Value = i->second.get_str();
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, int& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find(name);
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::int_type != i->second.type() )
					{
						return false;
					}
					else
					{
						Value = i->second.get_int();
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, WORD& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::int_type != i->second.type() )
					{
						return false;
					}
					else
					{
						int Temp = i->second.get_int();
						if ( Temp > USHRT_MAX )
						{
							return false;
						}
						else
						{
							Value = static_cast< WORD >( Temp );
							return true;
						}
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, DWORD& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::int_type != i->second.type() )
					{
						return false;
					}
					else
					{
						__int64 Temp = i->second.get_uint64();
						if ( Temp > UINT_MAX )
						{
							return false;
						}
						else
						{
							Value = static_cast< DWORD >( Temp );
							return true;
						}
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, __int64& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::int_type != i->second.type() )
					{
						return false;
					}
					else
					{
						Value = i->second.get_uint64();
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, unsigned long long& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::int_type != i->second.type() )
					{
						return false;
					}
					else
					{
						Value = i->second.get_uint64();
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, bool& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::bool_type != i->second.type() )
					{
						return false;
					}
					else
					{
						Value = i->second.get_bool();
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, double& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::real_type != i->second.type() )
					{
						return false;
					}
					else
					{
						Value = i->second.get_real();
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}	
		}

		template<>
		static bool find_value(const json_spirit::mObject& obj, const std::string& name, float& Value)
		{
			try
			{
				json_spirit::mObject::const_iterator i = obj.find( name );
				if ( i == obj.end() || i->first != name )
				{
					return false;
				}
				else
				{
					if ( json_spirit::real_type != i->second.type() )
					{
						return false;
					}
					else
					{
						Value = static_cast< float >( i->second.get_real() );
						return true;
					}
				}
			}
			catch (...)
			{
				return false;
			}
		}

	};

} // namespace sc

#endif // _MIN_JSON_H_