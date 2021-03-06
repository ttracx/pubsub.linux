
#include "hpb_client_test.h"
#include "hpb_test_utils.h"


static HLByte CLIENT1_HYPE_ID[] = "\x85\xa9\xd4\xc4\xde\xd2\x87\x75\x0f\xc0\xed\x32";
static HLByte CLIENT2_HYPE_ID[] = "\x66\xd8\xf2\x20\x6a\x56\xdb\xe9\x91\x23\x3b\xc2";
static HLByte CLIENT3_HYPE_ID[] = "\xe7\x79\x34\x6c\x66\x9c\x17\xf4\x34\xc8\xce\x0e";

static HLByte CLIENT1_KEY[] = "\x05\xeb\x63\x7c\xbd\x3f\x33\x69\x1d\x74\x3c\x2a\x39\xaf\xee\xda\x5e\xc9\x45\xad"; // sha1 hash of the hype id
static HLByte CLIENT2_KEY[] = "\xf6\xcb\x6d\x9d\xb0\x98\x91\x9b\x2d\x39\x55\x11\x41\xc5\xcb\xe7\x67\xb5\x06\xd6"; // sha1 hash of the hype id
static HLByte CLIENT3_KEY[] = "\xe4\x9a\xa7\x79\x2c\xf4\xfd\x09\x6c\x10\x3f\x4b\xa4\x63\xe2\x7b\x91\x60\x9e\x6b"; // sha1 hash of the hype id

static HypeInstance * hpb_client_test_dummy_hype_instance_create(HLByte *id, size_t id_size);
static void hpb_client_test_dummy_hype_instance_release(HypeInstance *instance);

void hpb_client_test()
{
    HpbClient *cl1 = hpb_test_utils_get_client_from_id(CLIENT1_HYPE_ID, HPB_UTILS_CLIENT_ID_TEST_SIZE);
    HpbClient *cl2 = hpb_test_utils_get_client_from_id(CLIENT2_HYPE_ID, HPB_UTILS_CLIENT_ID_TEST_SIZE);
    HpbClient *cl3 = hpb_test_utils_get_client_from_id(CLIENT3_HYPE_ID, HPB_UTILS_CLIENT_ID_TEST_SIZE);

    // Test struct creation
    CU_ASSERT_PTR_NOT_NULL_FATAL(cl1);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cl2);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cl3);

    // Validate the IDs and keys of the structs created
    CU_ASSERT_NSTRING_EQUAL(cl1->hype_instance->identifier->data, CLIENT1_HYPE_ID, HPB_UTILS_CLIENT_ID_TEST_SIZE);
    CU_ASSERT_NSTRING_EQUAL(cl2->hype_instance->identifier->data, CLIENT2_HYPE_ID, HPB_UTILS_CLIENT_ID_TEST_SIZE);
    CU_ASSERT_NSTRING_EQUAL(cl3->hype_instance->identifier->data, CLIENT3_HYPE_ID, HPB_UTILS_CLIENT_ID_TEST_SIZE);
    CU_ASSERT_NSTRING_EQUAL(cl1->key, CLIENT1_KEY, SHA1_BLOCK_SIZE);
    CU_ASSERT_NSTRING_EQUAL(cl2->key, CLIENT2_KEY, SHA1_BLOCK_SIZE);
    CU_ASSERT_NSTRING_EQUAL(cl3->key, CLIENT3_KEY, SHA1_BLOCK_SIZE);

    // Test hpb_client_is_id_equal() method
    HpbClient *cl4 = hpb_test_utils_get_client_from_id(CLIENT1_HYPE_ID, HPB_UTILS_CLIENT_ID_TEST_SIZE);
    CU_ASSERT_PTR_NOT_NULL_FATAL(cl4);
    CU_ASSERT_FALSE(hpb_client_is_instance_equal(cl1, cl2->hype_instance));
    CU_ASSERT_TRUE(hpb_client_is_instance_equal(cl1, cl4->hype_instance));

    // Test struct destruction
    hpb_client_destroy(&cl1);
    hpb_client_destroy(&cl2);
    hpb_client_destroy(&cl3);
    hpb_client_destroy(&cl4);
    CU_ASSERT_PTR_NULL(cl1);
    CU_ASSERT_PTR_NULL(cl2);
    CU_ASSERT_PTR_NULL(cl3);
    CU_ASSERT_PTR_NULL(cl4);
}
