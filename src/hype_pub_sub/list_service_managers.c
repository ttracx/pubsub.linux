
#include "hype_pub_sub/list_service_managers.h"

ListServiceManagers *hype_pub_sub_list_service_managers_create()
{
    return linked_list_create();
}

int hype_pub_sub_list_service_managers_add(ListServiceManagers *list_serv_man, byte service_key[SHA1_BLOCK_SIZE])
{
    if(hype_pub_sub_list_service_managers_is_inserted(list_serv_man, service_key))
        return -1;

    ServiceManager *serv_man = hype_pub_sub_service_manager_create(service_key);
    return linked_list_add(list_serv_man, serv_man);
}

int hype_pub_sub_list_service_managers_remove(ListServiceManagers **list_serv_man, byte service_key[])
{
    return linked_list_remove(list_serv_man, service_key, compare_service_manager, free_service_manager);
}

void hype_pub_sub_list_service_managers_destroy(ListServiceManagers *list_serv_man)
{
    linked_list_destroy(list_serv_man, free_service_manager);
}

bool hype_pub_sub_list_service_managers_is_inserted(ListServiceManagers* list_serv_man, byte service_key[])
{
    return linked_list_is_element_inserted(list_serv_man, service_key, compare_service_manager);
}

bool compare_service_manager(void *serv_man1, void *serv_man2)
{
    if (serv_man1 == NULL || serv_man2 == NULL)
        return false;

    if(memcmp(((ServiceManager*) serv_man1)->service_key, (byte*) serv_man2, SHA1_BLOCK_SIZE * sizeof(byte)) == 0)
        return true;

    return false;
}

void free_service_manager(void *serv_man)
{
    hype_pub_sub_service_manager_destroy((ServiceManager*) serv_man);
}