// ----------------------------------------------------------------------------
// Copyright 2017-2019 ARM Ltd.
//  
// Licensed under the Apache License, Version 2.0 (the "License");
// you may not use this file except in compliance with the License.
// You may obtain a copy of the License at
//  
//     http://www.apache.org/licenses/LICENSE-2.0
//  
// Unless required by applicable law or agreed to in writing, software
// distributed under the License is distributed on an "AS IS" BASIS,
// WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
// See the License for the specific language governing permissions and
// limitations under the License.
// ----------------------------------------------------------------------------


#include "unity_fixture.h"
#include "sda_log.h"
#include "cose.h"
#include "cose_int.h"
#include "sda_internal_defs.h"
#include "secure_device_access.h"
#include "fcc_status.h"
#include "sda_verification.h"
#include "test_utils.h"
#include "sda_trust_anchor.h"
#include  "tinycbor.h"
#include "test_common_utils.h"

const char test_device_id_string[] = "f90b1017e52f4c70ad92684e802c9249";
const char test_endpoint_name_string[] = "my-device-1";

const uint8_t secp256r1_der_key_1[SDA_TRUST_ANCHOR_SIZE] =
{ 0x30, 0x59, 0x30, 0x13, 0x06, 0x07, 0x2a, 0x86, 0x48, 0xce, 0x3d, 0x02, 0x01, 0x06, 0x08, 0x2a,
0x86, 0x48, 0xce, 0x3d, 0x03, 0x01, 0x07, 0x03, 0x42, 0x00, 0x04, 0xa4, 0x9d, 0xa7, 0xfc, 0xf9,
0x40, 0x5f, 0xa1, 0x52, 0xcf, 0xea, 0x97, 0x54, 0x8a, 0xc0, 0xae, 0x60, 0x04, 0x24, 0x85, 0x8a,
0xfc, 0x3a, 0xc3, 0xe5, 0x0d, 0x44, 0xd0, 0xe2, 0x55, 0x0d, 0x34, 0x51, 0xb0, 0x22, 0xe4, 0xb4,
0xa9, 0x74, 0x25, 0x1a, 0x4e, 0x52, 0xe4, 0xcd, 0x5a, 0x85, 0x06, 0x5d, 0x66, 0x94, 0x58, 0x46,
0x8f, 0x55, 0xa3, 0xe0, 0x50, 0x7e, 0xee, 0xa4, 0x2e, 0x5f, 0x25 };

const uint8_t secp256r1_der_key_1_name[TEST_SDA_TRUST_ANCHOR_KEY_NAME_SIZE] = "mbed.ta.CAC252CE379CC0B2DA9CFD9B3095ADDF66EDF0DFD150A3661E6E5C24B26B821C";

TEST_GROUP(audience_test);


TEST_SETUP(audience_test)
{
    fcc_tst_setup();
    TEST_SKIP_EXECUTION_ON_FAILURE();

}

TEST_TEAR_DOWN(audience_test)
{
    fcc_tst_tear_down();
    TEST_SKIP_EXECUTION_ON_FAILURE();
}

TEST(audience_test, verify_single_endpoint_name_audience_test)
{
    CborParser parser;
    CborValue audience_array;
    CborError cbor_error = CborNoError;

    const uint8_t wrong_audience_string_1[] =
    //["endpoint-name:device-14532345345"]
    { 0x81, 0x78, 0x20, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A, 0x64, 0x65,
      0x76, 0x69, 0x63, 0x65, 0x2D, 0x31, 0x34, 0x35, 0x33, 0x32, 0x33, 0x34, 0x35, 0x33, 0x34, 0x35 };

    const uint8_t wrong_audience_string_2[] =
    //["endpoint-name:my-device-154l25lggs3452345234458567984657356473456245345634563546745678456845678567846785678467856785678335463452345556735467345345634563456d"]
    { 0x81, 0x78, 0x9C, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A, 0x6D, 0x79,
      0x2D, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x31, 0x35, 0x34, 0x6C, 0x32, 0x35, 0x6C, 0x67, 0x67, 0x73, 0x33,
      0x34, 0x35, 0x32, 0x33, 0x34, 0x35, 0x32, 0x33, 0x34, 0x34, 0x35, 0x38, 0x35, 0x36, 0x37, 0x39, 0x38, 0x34, 0x36,
      0x35, 0x37, 0x33, 0x35, 0x36, 0x34, 0x37, 0x33, 0x34, 0x35, 0x36, 0x32, 0x34, 0x35, 0x33, 0x34, 0x35, 0x36, 0x33,
      0x34, 0x35, 0x36, 0x33, 0x35, 0x34, 0x36, 0x37, 0x34, 0x35, 0x36, 0x37, 0x38, 0x34, 0x35, 0x36, 0x38, 0x34, 0x35,
      0x36, 0x37, 0x38, 0x35, 0x36, 0x37, 0x38, 0x34, 0x36, 0x37, 0x38, 0x35, 0x36, 0x37, 0x38, 0x34, 0x36, 0x37, 0x38,
      0x35, 0x36, 0x37, 0x38, 0x35, 0x36, 0x37, 0x38, 0x33, 0x33, 0x35, 0x34, 0x36, 0x33, 0x34, 0x35, 0x32, 0x33, 0x34,
      0x35, 0x35, 0x35, 0x36, 0x37, 0x33, 0x35, 0x34, 0x36, 0x37, 0x33, 0x34, 0x35, 0x33, 0x34, 0x35, 0x36, 0x33, 0x34,
      0x35, 0x36, 0x33, 0x34, 0x35, 0x36, 0x64 };

    const uint8_t empty_audience_string[] = {0x81, 0x60 };

    const uint8_t audience_string[] = 
    { 0x81, 0x78, 0x19, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A, 0x6D, 0x79,
      0x2D, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x31 };

    sda_status_internal_e sda_status_internal = SDA_STATUS_INTERNAL_SUCCESS;
    sda_status_e sda_status = SDA_STATUS_SUCCESS;


    //Try to call sda_verify_audience when device id wasn't set
     sda_status_internal = sda_audience_verify_tiny(audience_string, sizeof(audience_string));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);

    //Set endpoint name to the device
    sda_status = test_provisioning_setup(false, secp256r1_der_key_1_name, sizeof(secp256r1_der_key_1_name), secp256r1_der_key_1, sizeof(secp256r1_der_key_1), NULL, 0, test_endpoint_name_string, strlen(test_endpoint_name_string));
    TEST_ASSERT_EQUAL_INT(SDA_STATUS_SUCCESS, sda_status);

    //Verify the same audience
    sda_status_internal = sda_audience_verify_tiny(audience_string, sizeof(audience_string));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_SUCCESS);
    //Free decoded_audience_array for the next case
 

    //verify wrong audience
    sda_status_internal = sda_audience_verify_tiny(wrong_audience_string_1, sizeof(wrong_audience_string_1));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);



    //verify wrong audience
    sda_status_internal = sda_audience_verify_tiny(wrong_audience_string_2, sizeof(wrong_audience_string_2));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);



    //Verify empty audience
    sda_status_internal = sda_audience_verify_tiny(empty_audience_string, sizeof(empty_audience_string));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);

}


TEST(audience_test, verify_single_device_id_audience_test)
{

    CborParser parser;
    CborValue audience_array;
    CborError cbor_error = CborNoError;

    const char wrong_audience_string_1[] = 
    //["device-id:ad0b1017e52f4c70ad92684e802c9249"]
    { 0x81, 0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x61, 0x64, 0x30, 0x62, 0x31, 0x30,
      0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38,
      0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39 };
    const char wrong_audience_string_2[] = 
    //["device-id:f90b1017e52f4c70ad92684e802c9255"]
    { 0x81, 0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x66, 0x39, 0x30, 0x62, 0x31, 0x30,
      0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38,
      0x30, 0x32, 0x63, 0x39, 0x32, 0x35, 0x35
    };
    const char audience_string[] = 
    //["id:f90b1017e52f4c70ad92684e802c9249"]
    { 0x81, 0x78, 0x23, 0x69, 0x64, 0x3A, 0x66, 0x39, 0x30, 0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34,
      0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39 };
    const char endpoint_name[] = "my-device-1";
    sda_status_internal_e sda_status_internal = SDA_STATUS_INTERNAL_SUCCESS;
    sda_status_e sda_status = SDA_STATUS_SUCCESS;
    const char device_id_string[] = "f90b1017e52f4c70ad92684e802c9249";

    //Initialize cbor parser with encoded buffer
    //Try to call sda_verify_audience when device id wasn't set
    sda_status_internal = sda_audience_verify_tiny(wrong_audience_string_1, sizeof(wrong_audience_string_1));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);

    sda_status = test_provisioning_setup(false, secp256r1_der_key_1_name, sizeof(secp256r1_der_key_1_name), secp256r1_der_key_1, sizeof(secp256r1_der_key_1), device_id_string, strlen(device_id_string), test_endpoint_name_string, strlen(test_endpoint_name_string));
    TEST_ASSERT_EQUAL_INT(SDA_STATUS_SUCCESS, sda_status);

    //Verify the same audience
    sda_status_internal = sda_audience_verify_tiny(audience_string, sizeof(audience_string));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_SUCCESS);

    //Verify wrong_audience_string_1
    sda_status_internal = sda_audience_verify_tiny(wrong_audience_string_1, sizeof(wrong_audience_string_1));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);

    //Verify wrong_audience_string_2
    sda_status_internal = sda_audience_verify_tiny(wrong_audience_string_2, sizeof(wrong_audience_string_2));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);
}

TEST(audience_test, verify_multiple_audience_test)
{
    CborParser parser;
    CborValue audience_array;
    CborError cbor_error = CborNoError;


    const uint8_t wrong_audience_string_1[] = 
    //["device-id:ad0b1017e52f4c70ad92684e802c9249"]
    { 0x81, 0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x61, 0x64, 0x30, 0x62, 0x31, 0x30,
      0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38,
      0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39};
    const uint8_t wrong_audience_string_2[] =
    //["device-id:f90b1017e52f4c70ad92684e802c9255"]
    { 0x81, 0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x66, 0x39, 0x30, 0x62, 0x31, 0x30,
      0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38,
      0x30, 0x32, 0x63, 0x39, 0x32, 0x35, 0x35 };
    const uint8_t device_audience_string_1[] =
    //["id:545b1017e52f4c70ad92684e802c9249", "id:f90b1017e5834c70ad92684e802c9249", "id:f90b1017e52f4c70ad92684e802c9249"]
    { 0x83, 0x78, 0x23, 0x69, 0x64, 0x3A, 0x35, 0x34, 0x35, 0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34,
      0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39,
      0x78, 0x23, 0x69, 0x64, 0x3A, 0x66, 0x39, 0x30, 0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35, 0x38, 0x33, 0x34, 0x63,
      0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39, 0x78,
      0x23, 0x69, 0x64, 0x3A, 0x66, 0x39, 0x30, 0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34, 0x63, 0x37,
      0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39 };
    const uint8_t endpoint_name_audience_string_2[] =
    //["endpoint-name:device-14532345345", "endpoint-name:my-device-4", "endpoint-name:my-device-1"]
    { 0x83, 0x78, 0x20, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A, 0x64, 0x65,
      0x76, 0x69, 0x63, 0x65, 0x2D, 0x31, 0x34, 0x35, 0x33, 0x32, 0x33, 0x34, 0x35, 0x33, 0x34, 0x35, 0x78, 0x19, 0x65,
      0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A, 0x6D, 0x79, 0x2D, 0x64, 0x65, 0x76,
      0x69, 0x63, 0x65, 0x2D, 0x34, 0x78, 0x19, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D,
      0x65, 0x3A, 0x6D, 0x79, 0x2D, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x31 };
    const uint8_t mixed_audience_string[] =
    //["ep:device-14532345345", "device-id:545b1017e52f4c70ad92684e802c9249", "ep:my-device-4", "device-id:f90b1017e5834c70ad92684e802c9249", "device-id:f90b1017e52f4c70ad92684e802c9249", "ep:my-device-1"]
    { 0x86, 0x75, 0x65, 0x70, 0x3A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x31, 0x34, 0x35, 0x33, 0x32, 0x33, 0x34,
      0x35, 0x33, 0x34, 0x35, 0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x35, 0x34, 0x35,
      0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38,
      0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39, 0x6E, 0x65, 0x70, 0x3A, 0x6D, 0x79, 0x2D, 0x64, 0x65,
      0x76, 0x69, 0x63, 0x65, 0x2D, 0x34, 0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x66,
      0x39, 0x30, 0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35, 0x38, 0x33, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32,
      0x36, 0x38, 0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39, 0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63,
      0x65, 0x2D, 0x69, 0x64, 0x3A, 0x66, 0x39, 0x30, 0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34, 0x63,
      0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x39, 0x6E,
      0x65, 0x70, 0x3A, 0x6D, 0x79, 0x2D, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x31
    };
const char mixed_audience_string_2[] = 
    //["endpoint-name:device-14532345345", "device-id:545b1017e52f4c70ad92684e802c9249", "endpoint-name:my-device-4", "endpoint-name:my-device-1", "device-id:f90b1017e5834c70ad92684e802c9249", "device-id:f90b1017e52f4c70ad92684e802c9245"]
    { 0x86, 0x78, 0x20, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A, 0x64, 0x65,
      0x76, 0x69, 0x63, 0x65, 0x2D, 0x31, 0x34, 0x35, 0x33, 0x32, 0x33, 0x34, 0x35, 0x33, 0x34, 0x35, 0x78, 0x2A, 0x64,
      0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x35, 0x34, 0x35, 0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35,
      0x32, 0x66, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39,
      0x32, 0x34, 0x39, 0x78, 0x19, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69, 0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A,
      0x6D, 0x79, 0x2D, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x34, 0x78, 0x19, 0x65, 0x6E, 0x64, 0x70, 0x6F, 0x69,
      0x6E, 0x74, 0x2D, 0x6E, 0x61, 0x6D, 0x65, 0x3A, 0x6D, 0x79, 0x2D, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x31,
      0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x66, 0x39, 0x30, 0x62, 0x31, 0x30, 0x31,
      0x37, 0x65, 0x35, 0x38, 0x33, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32, 0x36, 0x38, 0x34, 0x65, 0x38, 0x30,
      0x32, 0x63, 0x39, 0x32, 0x34, 0x39, 0x78, 0x2A, 0x64, 0x65, 0x76, 0x69, 0x63, 0x65, 0x2D, 0x69, 0x64, 0x3A, 0x66,
      0x39, 0x30, 0x62, 0x31, 0x30, 0x31, 0x37, 0x65, 0x35, 0x32, 0x66, 0x34, 0x63, 0x37, 0x30, 0x61, 0x64, 0x39, 0x32,
      0x36, 0x38, 0x34, 0x65, 0x38, 0x30, 0x32, 0x63, 0x39, 0x32, 0x34, 0x35 };
    sda_status_internal_e sda_status_internal = SDA_STATUS_INTERNAL_SUCCESS;
	sda_status_e sda_status = SDA_STATUS_SUCCESS;

    //Set device id and endpoint name to the device
    sda_status = test_provisioning_setup(false, secp256r1_der_key_1_name, sizeof(secp256r1_der_key_1_name), secp256r1_der_key_1, sizeof(secp256r1_der_key_1), test_device_id_string, strlen(test_device_id_string), "my-device-4", (size_t)strlen("my-device-4"));
    TEST_ASSERT_EQUAL_INT(SDA_STATUS_SUCCESS, sda_status);

    //Verify the audience
    sda_status_internal = sda_audience_verify_tiny(device_audience_string_1, sizeof(device_audience_string_1));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_SUCCESS);


    //Verify wrong_audience_string_1
    sda_status_internal = sda_audience_verify_tiny(wrong_audience_string_1, sizeof(wrong_audience_string_1));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);

    //Verify wrong_audience_string_2
    sda_status_internal = sda_audience_verify_tiny(wrong_audience_string_2, sizeof(wrong_audience_string_2));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_AUDIENCE_VERIFICATION_ERROR);

    //Try to verify the same audience
    sda_status_internal = sda_audience_verify_tiny(endpoint_name_audience_string_2, sizeof(endpoint_name_audience_string_2));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_SUCCESS);

    //Try to verify mixed audience string 
    sda_status_internal = sda_audience_verify_tiny(mixed_audience_string, sizeof(mixed_audience_string));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_SUCCESS);


    //Try to verify mixed audience string 
    sda_status_internal = sda_audience_verify_tiny(mixed_audience_string_2, sizeof(mixed_audience_string_2));
    TEST_ASSERT(sda_status_internal == SDA_STATUS_INTERNAL_SUCCESS);

}

TEST_GROUP_RUNNER(audience_test)
{
    RUN_TEST_CASE(audience_test, verify_single_device_id_audience_test);
    RUN_TEST_CASE(audience_test, verify_single_endpoint_name_audience_test);
    RUN_TEST_CASE(audience_test, verify_multiple_audience_test);

}