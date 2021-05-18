To parse `PropertiesFileFormat` there is `PropertiesFileParser` in:

	PropertiesFileParser.h
	PropertiesFileParser.c

# Example parse

This example parses a buffer and searches for properties with name `title` of simple type string and prints them. 

It also shows how to handle errors. 

	struct CArray properties_array;
	CArray_InitZero(&properties_array);
	if(caNoError == CArray_Create(&properties_array, 1, sizeof(struct TPFPProperty), TPFPProperty_Clear))
	{
		struct TPFPErrorData error;
		TPFPErrorData_InitZero(&error);

		int success = PFP_ParseFromMemory(buffer, &properties_array, &error);
		if (success)
		{
			for (size_t prop_i = 0; prop_i < properties_array.count; ++prop_i)
			{
				struct TPFPProperty* property = NULL;
				ca_error = CArray_At(&properties_array, prop_i, &property);
				if (caNoError == ca_error)
				{
					if (strcmp(property->name, "title") == 0)
						{
							if (pfpSimpleValue == property->type && pfpStringSimpleValue == property->value.simpleValue.type)
							{
								printf("title: %s\n", property->value.simpleValue.data.stringData);
							}
				}
			}

			CArray_Clear(&properties_array);
		}
		else
		{
			size_t error_msg_length = PFP_GetErrorStrSize(&error);
			if(0 < error_msg_length)
			{
				char* error_msg = malloc(error_msg_length);
				if(error_msg)
				{
					PFP_GetErrorStr(&error, error_msg);
					printf("%s\n", error_msg);
					free(error_msg);
				}
			}
		}
	}