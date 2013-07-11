/**
 * @author: Jeff Thompson
 * See COPYING for copyright and distribution information.
 */

#include "errors.h"

char *ndn_getErrorString(int error)
{
  switch (error) {
  case NDN_ERROR_success:
    return      "success"; 
  case NDN_ERROR_element_of_value_is_not_a_decimal_digit:
    return      "element of value is not a decimal digit";
  case NDN_ERROR_read_past_the_end_of_the_input:
    return      "read past the end of the input";
  case NDN_ERROR_the_first_header_octet_may_not_be_zero:
    return      "the first header octet may not be zero";
  case NDN_ERROR_header_type_is_not_a_DTAG:
    return      "header type is not a DTAG";
  case NDN_ERROR_did_not_get_the_expected_DTAG:
    return      "did not get the expected DTAG";
  case NDN_ERROR_did_not_get_the_expected_element_close:
    return      "did not get the expected element close";
  case NDN_ERROR_item_is_not_UDATA:
    return      "item is not UDATA";
  case NDN_ERROR_header_type_is_out_of_range:
    return      "header type is out of range";
  case NDN_ERROR_encodeTypeAndValue_miscalculated_N_encoding_bytes:
    return      "encodeTypeAndValue miscalculated N encoding bytes";
  case NDN_ERROR_read_a_component_past_the_maximum_number_of_components_allowed_in_the_name:
    return      "read a component past the maximum number of components allowed in the name";
  case NDN_ERROR_read_an_entry_past_the_maximum_number_of_entries_allowed_in_the_exclude:
    return      "read an entry past the maximum number of entries allowed in the exclude";
  case NDN_ERROR_findElementEnd_unexpected_close_tag:
    return      "findElementEnd unexpected close tag";
  case NDN_ERROR_cannot_store_more_header_bytes_than_the_size_of_headerBuffer:
    return      "cannot store more header bytes than the size of headerBuffer";
  case NDN_ERROR_findElementEnd_cannot_read_header_type_and_value:
    return      "findElementEnd cannot read header type and value";
  case NDN_ERROR_findElementEnd_unrecognized_header_type:
    return      "findElementEnd unrecognized header type";
  case NDN_ERROR_findElementEnd_unrecognized_state:
    return      "findElementEnd unrecognized state";
  case NDN_ERROR_DynamicUCharArray_realloc_function_pointer_not_supplied:
    return      "DynamicUCharArray realloc function pointer not supplied";
  case NDN_ERROR_DynamicUCharArray_realloc_failed:
    return      "DynamicUCharArray realloc failed";
  case NDN_ERROR_unrecognized_ndn_ExcludeType:
    return      "unrecognized ndn_ExcludeType";
  case NDN_ERROR_unrecognized_ndn_KeyLocatorType:
    return      "unrecognized ndn_KeyLocatorType";
  case NDN_ERROR_decodeBinaryXMLKeyLocator_unrecognized_key_locator_type:
    return      "decodeBinaryXMLKeyLocator unrecognized key locator type";
  default:
    return "unrecognized ndn_Error code";  
  }
}
