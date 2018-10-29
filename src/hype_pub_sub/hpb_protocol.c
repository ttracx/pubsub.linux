
#include "hype_pub_sub/hpb_protocol.h"

//
// Static functions declaration
//

static size_t hpb_protocol_build_packet(HLByte ** packet,int n_fields, ...);
static int hpb_protocol_receive_subscribe_msg(HpbProtocol *protocol, HypeInstance * instance_origin, HLByte *msg, size_t msg_length);
static int hpb_protocol_receive_unsubscribe_msg(HpbProtocol *protocol, HypeInstance * instance_origin, HLByte *msg, size_t msg_length);
static int hpb_protocol_receive_publish_msg(HpbProtocol *protocol, HypeInstance * instance_origin, HLByte *msg, size_t msg_length);
static int hpb_protocol_receive_info_msg(HpbProtocol *protocol, HLByte *msg, size_t msg_length);
static MessageType hpb_protocol_get_message_type(HLByte *msg);

//
// Header functions implementation
//

HpbProtocol *hpb_protocol_create(struct HypePubSub_ *hpb)
{
    HpbProtocol *prot = (HpbProtocol *) malloc(sizeof(HpbProtocol));
    prot->hpb = hpb;
    return prot;
}

HLByte *hpb_protocol_send_subscribe_msg(HLByte service_key[], HypeInstance * instance_dest)
{
    HLByte type = (HLByte) SUBSCRIBE_SERVICE;
    HpbProtocolPacketField msg_type_field = {&type, MESSAGE_TYPE_BYTE_SIZE };
    HpbProtocolPacketField ser_key_field = {service_key, SHA1_BLOCK_SIZE };
    size_t n_fields = 2;
    HLByte *packet;


    size_t size = hpb_protocol_build_packet(&packet,n_fields, &msg_type_field, &ser_key_field);

    hype_send(packet,size,instance_dest,false);

    // TODO: msg memory must freed
    return packet;
}

HLByte *hpb_protocol_send_unsubscribe_msg(HLByte service_key[], HypeInstance * instance_dest)
{
    HLByte type = (HLByte) UNSUBSCRIBE_SERVICE;
    HpbProtocolPacketField msg_type_field = {&type, MESSAGE_TYPE_BYTE_SIZE };
    HpbProtocolPacketField ser_key_field = {service_key, SHA1_BLOCK_SIZE };
    size_t n_fields = 2;
    HLByte *packet;

    size_t size = hpb_protocol_build_packet(&packet,n_fields, &msg_type_field, &ser_key_field);

    hype_send(packet,size,instance_dest,false);

    return packet;
}

HLByte *hpb_protocol_send_publish_msg(HLByte service_key[], HypeInstance * instance_dest, char *msg, size_t msg_length)
{
    HLByte type = (HLByte) PUBLISH;
    HpbProtocolPacketField msg_type_field = {&type, MESSAGE_TYPE_BYTE_SIZE };
    HpbProtocolPacketField ser_key_field = {service_key, SHA1_BLOCK_SIZE };
    HpbProtocolPacketField msg_field = {(HLByte *) msg, msg_length };
    size_t n_fields = 3;

    HLByte *packet;

    size_t size = hpb_protocol_build_packet(&packet,n_fields, &msg_type_field, &ser_key_field);

    hype_send(packet,size,instance_dest,false);

    // TODO: msg memory must freed
    return packet;
}

HLByte *hpb_protocol_send_info_msg(HLByte service_key[], HypeInstance * instance_dest, char *msg, size_t msg_length)
{
    HLByte type = (HLByte) INFO;
    HpbProtocolPacketField msg_type_field = {&type, MESSAGE_TYPE_BYTE_SIZE };
    HpbProtocolPacketField ser_key_field = {service_key, SHA1_BLOCK_SIZE };
    HpbProtocolPacketField msg_field = {(HLByte *) msg, msg_length };
    size_t n_fields = 3;

    HLByte *packet;

    size_t size = hpb_protocol_build_packet(&packet,n_fields, &msg_type_field, &ser_key_field);

    hype_send(packet,size,instance_dest,false);

    // TODO: msg memory must freed
    return packet;
}

int hpb_protocol_receive_msg(HpbProtocol *protocol, HypeInstance * instance_origin, HLByte *msg, size_t msg_length)
{
    if(msg_length <= 0) {
        return -1;
    }

    int m_type = hpb_protocol_get_message_type(msg);

    switch (m_type)
    {
        case SUBSCRIBE_SERVICE:
            hpb_protocol_receive_subscribe_msg(protocol, instance_origin, msg, msg_length);
            break;
        case UNSUBSCRIBE_SERVICE:
            hpb_protocol_receive_unsubscribe_msg(protocol, instance_origin, msg, msg_length);
            break;
        case PUBLISH:
            hpb_protocol_receive_publish_msg(protocol, instance_origin, msg, msg_length);
            break;
        case INFO:
            hpb_protocol_receive_info_msg(protocol, msg, msg_length);
            break;
        case INVALID:
            return -1; // Message type not recognized. Discard
    }

    return 0;
}

void hpb_protocol_destroy(HpbProtocol **protocol)
{
    free(*protocol);
    (*protocol) = NULL;
}

//
// Static functions implementation
//

static size_t hpb_protocol_build_packet(HLByte ** packet,int n_fields, ...)
{
    if(n_fields <=0) {
        return 0;
    }

    va_list p_fields;
    size_t p_size = 0;

    va_start(p_fields, n_fields);

    // Get full packet size
    for (int i = 0; i < n_fields; i++){
        p_size += va_arg(p_fields, HpbProtocolPacketField*)->size;
    }

    *packet = (HLByte*) malloc(p_size * sizeof(HLByte));

    va_start(p_fields, n_fields); // re-initialize the list to re-iterate

    // Build packet by joining the fields
    size_t bytes_written = 0;

    for (int i = 0; i < n_fields; i++)
    {
        HpbProtocolPacketField *field = va_arg(p_fields, HpbProtocolPacketField*);
        memmove(*packet+bytes_written, field->data, field->size);
        bytes_written += field->size;
    }

    va_end(p_fields);
    return p_size;
}

static MessageType hpb_protocol_get_message_type(HLByte *msg)
{
    if(msg[0] == (HLByte) SUBSCRIBE_SERVICE)
        return SUBSCRIBE_SERVICE;
    else if(msg[0] == (HLByte) UNSUBSCRIBE_SERVICE)
        return UNSUBSCRIBE_SERVICE;
    else if(msg[0] == (HLByte) PUBLISH)
        return PUBLISH;
    else if(msg[0] == (HLByte) INFO)
        return INFO;
    else
        return INVALID; // This should never happen
}

static int hpb_protocol_receive_subscribe_msg(HpbProtocol *protocol, HypeInstance * instance_origin, HLByte *msg, size_t msg_length)
{
    if(msg_length != (MESSAGE_TYPE_BYTE_SIZE + SHA1_BLOCK_SIZE)) {
        return -1; // Invalid lenght for a subscribe message
    }

    HLByte *service_key = (HLByte *) malloc(SHA1_BLOCK_SIZE * sizeof(char));
    memmove(service_key,msg+MESSAGE_TYPE_BYTE_SIZE, SHA1_BLOCK_SIZE);
    hpb_process_subscribe_req(service_key, instance_origin);
    return 0;
}

static int hpb_protocol_receive_unsubscribe_msg(HpbProtocol *protocol, HypeInstance * instance_origin, HLByte *msg, size_t msg_length)
{
    if(msg_length != (MESSAGE_TYPE_BYTE_SIZE + SHA1_BLOCK_SIZE)) {
        return -1; // Invalid lenght for a unsubscribe message
    }

    HLByte *service_key = (HLByte *) malloc(SHA1_BLOCK_SIZE * sizeof(char));
    memmove(service_key,msg+MESSAGE_TYPE_BYTE_SIZE, SHA1_BLOCK_SIZE);
    hpb_process_unsubscribe_req(service_key, instance_origin);
    return 0;
}

static int hpb_protocol_receive_publish_msg(HpbProtocol *protocol, HypeInstance * instance_origin, HLByte *msg, size_t msg_length)
{
    if(msg_length <= (MESSAGE_TYPE_BYTE_SIZE + SHA1_BLOCK_SIZE)) {
        return -1; // Invalid lenght for a publish message
    }

    HLByte *service_key = (HLByte *) malloc(SHA1_BLOCK_SIZE * sizeof(char));
    memmove(service_key,msg+MESSAGE_TYPE_BYTE_SIZE, SHA1_BLOCK_SIZE);
    size_t msg_content_size = msg_length - MESSAGE_TYPE_BYTE_SIZE - SHA1_BLOCK_SIZE;
    char *msg_content = (char *) malloc(msg_content_size* sizeof(char));
    memmove(msg_content, (msg + MESSAGE_TYPE_BYTE_SIZE + SHA1_BLOCK_SIZE), msg_content_size);
    hpb_process_publish_req(service_key, msg_content, msg_content_size);
    return 0;
}

static int hpb_protocol_receive_info_msg(HpbProtocol *protocol, HLByte *msg, size_t msg_length)
{
    if(msg_length <= (MESSAGE_TYPE_BYTE_SIZE + SHA1_BLOCK_SIZE))
        return -1; // Invalid lenght for a info message

    HLByte *service_key = (HLByte *) malloc(SHA1_BLOCK_SIZE * sizeof(char));
    memmove(service_key,msg+MESSAGE_TYPE_BYTE_SIZE, SHA1_BLOCK_SIZE);
    size_t msg_content_size = msg_length - MESSAGE_TYPE_BYTE_SIZE - SHA1_BLOCK_SIZE + 1; // +1 to add \0
    char *msg_content = (char *) malloc(msg_content_size* sizeof(char));
    memmove(msg_content, (msg + MESSAGE_TYPE_BYTE_SIZE + SHA1_BLOCK_SIZE), msg_content_size);
    msg_content[msg_content_size-1] = '\0';
    hpb_process_info_msg(service_key, msg_content, msg_content_size);

    return 0;
}