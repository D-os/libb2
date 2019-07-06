/*
 * Copyright (c) 2005 Palmsource, Inc.
 *
 * This software is licensed as described in the file LICENSE, which
 * you should have received as part of this distribution. The terms
 * are also available at http://www.openbinder.org/license.html.
 *
 * This software consists of voluntary contributions made by many
 * individuals. For the exact contribution history, see the revision
 * history and logs, available at http://www.openbinder.org
 */

#include <support/IStorage.h>
#include <support/Parcel.h>

#include <binder/Debug.h>
#include <stdlib.h>
#include <support/StdIO.h>
#include <support/StringUtils.h>

namespace os {
namespace support {

B_STATIC_STRING_VALUE_8(key_Size, "Size", );
B_STATIC_STRING_VALUE_8(key_SetSize, "SetSize", );
B_STATIC_STRING_VALUE_8(key_ReadAt, "ReadAt", );
B_STATIC_STRING_VALUE_8(key_WriteAt, "WriteAt", );
B_STATIC_STRING_VALUE_8(key_Sync, "Sync", );

/*-----------------------------------------------------------------*/

class BpStorage : public BpInterface<IStorage>
{
 public:
  BpStorage(const sptr<IBinder>& o)
      : BpInterface<IStorage>(o),
        m_canTransact(true)
  {
  }

  virtual off_t Size() const
  {
    status_t    err;
    const off_t off = 0;  //STUB
    STUB;                 //remote()->Get(key_Size).AsOffset(&err);
    return err >= B_OK ? off : err;
  }

  virtual status_t SetSize(off_t size)
  {
    STUB;
    return 0; /*STUB: remote()->Invoke(
                       key_Sync,
                       SValue(B_0_INT32, SValue::Offset(size)))
        .AsStatus();*/
  }

  virtual ssize_t ReadAtV(off_t position, const iovec* vector, ssize_t count)
  {
    SParcel replyParcel;
    SValue  replyValue;

    ssize_t        thisSize, size = 0;
    ssize_t        status   = B_OK;
    ssize_t        sizeLeft = 0;
    const uint8_t *buf = NULL, *pos = NULL;

    // Count total number of bytes being requested.
    for (int32_t i = 0; i < count; i++) size += vector[i].iov_len;

    // If we think the remote binder can handle low-level
    // transactions, go for it.
    if (m_canTransact) {
      // Command tell the target how many bytes to read.
      SParcel             command;
      read_at_transaction h;
      h.position = position;
      h.size     = size;
      command.setData(reinterpret_cast<const uint8_t*>(&h), sizeof(read_at_transaction));

      // Execute the read operation.
      status   = remote()->transact(B_READ_AT_TRANSACTION, command, &replyParcel);
      sizeLeft = replyParcel.dataSize();

      // Retrieve status and setup to read data.
      if (status >= B_OK && sizeLeft >= static_cast<ssize_t>(sizeof(ssize_t))) {
        buf = replyParcel.data();
        if (*reinterpret_cast<const ssize_t*>(buf) >= 0) {
          buf += sizeof(ssize_t);
          sizeLeft -= sizeof(ssize_t);
          pos = buf;
        }
        else {
          // Return error code.
          status = *reinterpret_cast<const ssize_t*>(buf);
        }
      }
      else if (status >= B_OK) {
        // Return error code.
        status = B_NO_MEMORY;
      }

      // If the remote didn't understand our transaction, revert
      // to the Effect() API and try again.
      if (status == B_BINDER_UNKNOWN_TRANSACT) {
        m_canTransact = false;
      }
    }

    // If we know the remote binder can't handle remote transactions
    // (either from an attempt we just did or a previous one), talk
    // with it through the high-level Effect() API.
    if (!m_canTransact) {
      // Execute the operation and setup to read data.
      STUB; /*replyValue = remote()->Invoke(
          key_ReadAt,
          SValue(B_0_INT32, SValue::Int64(position)).JoinItem(B_1_INT32, SValue::SSize(size)));
      status   = replyValue.ErrorCheck();
      sizeLeft = replyValue.dataSize();
      pos = buf = replyValue.data();*/
      if (status >= 0 && pos == NULL) status = B_NO_MEMORY;
    }

    // Retrieve data and write it out.
    if (status >= B_OK) {
      for (int32_t i = 0; i < count && sizeLeft > 0; i++) {
        thisSize = vector[i].iov_len;
        if (thisSize > sizeLeft) thisSize = sizeLeft;
        memcpy(vector[i].iov_base, pos, thisSize);
        sizeLeft -= thisSize;
        pos += thisSize;
      }
      status = pos - buf;
    }
    return status;
  }

  virtual ssize_t WriteAtV(off_t position, const iovec* vector, ssize_t count)
  {
    ssize_t status(B_OK);

    // If we think the remote binder can handle low-level
    // transactions, go for it.
    if (m_canTransact) {
      SParcel command;
      SParcel reply;

      ssize_t size = 0, i;

      // Count number of bytes being written.
      for (i = 0; i < count; i++) size += vector[i].iov_len;
      // Allocate space for total bytes plus the flags parameter.
      command.setDataSize(size + sizeof(write_at_transaction));
      uint8_t* buf = const_cast<uint8_t*>(command.data());
      if (buf) {
        reinterpret_cast<write_at_transaction*>(buf)->position = position;
        buf += sizeof(write_at_transaction);
        // Collect and copy in data.
        for (i = 0; i < count; i++) {
          memcpy(buf, vector[i].iov_base, vector[i].iov_len);
          buf += vector[i].iov_len;
        }
      }
      else {
        status = B_NO_MEMORY;
      }

      // Execute the transaction created above.
      if (status >= B_OK) {
        status = remote()->transact(B_WRITE_AT_TRANSACTION, command, &reply);
        if (status >= B_OK) {
          if (reply.dataSize() >= static_cast<ssize_t>(sizeof(ssize_t)))
            status = *reinterpret_cast<const ssize_t*>(reply.data());
          else
            status = B_ERROR;
        }
      }

      // If the remote didn't understand our transaction, revert
      // to the Effect() API and try again.
      if (status == B_BINDER_UNKNOWN_TRANSACT) {
        m_canTransact = false;
      }
    }

    // If we know the remote binder can't handle remote transactions
    // (either from an attempt we just did or a previous one), talk
    // with it through the high-level Effect() API.
    if (!m_canTransact) {
      SValue  data;
      ssize_t size = 0, i;

      // Count number of bytes being written.
      for (i = 0; i < count; i++) size += vector[i].iov_len;
      // Copy data into value.
      uint8_t* buf = NULL;  //STUB
      STUB;                 //static_cast<uint8_t*>(data.BeginEditBytes(B_RAW_TYPE, size));
      if (buf) {
        // Collect and copy in data.
        for (i = 0; i < count; i++) {
          memcpy(buf, vector[i].iov_base, vector[i].iov_len);
          buf += vector[i].iov_len;
        }

        // Invoke write command on remote binder.
        STUB; /*const SValue reply = remote()->Invoke(
            key_WriteAt,
            SValue(B_0_INT32, SValue::Int64(position)).JoinItem(B_0_INT32, data));
        status = reply.AsSSize();*/
      }
      else {
        status = B_NO_MEMORY;
      }
    }

    return status;
  }

  virtual status_t Sync()
  {
    STUB;
    return 0;  //STUB: remote()->Invoke(key_Sync, B_WILD_VALUE).AsStatus();
  }

  bool m_canTransact;
};

IMPLEMENT_META_INTERFACE(Storage, "org.openbinder.support.IStorage")

/*-----------------------------------------------------------------*/

status_t
BnStorage::Transact(uint32_t code, SParcel& data, SParcel* reply, uint32_t flags)
{
  if (code == B_READ_AT_TRANSACTION) {
    ssize_t amt = 0;  //STUB
    STUB;             //B_BINDER_BAD_TRANSACT;
    if (data.dataSize() >= static_cast<ssize_t>(sizeof(read_at_transaction))) {
      const read_at_transaction* rd = reinterpret_cast<const read_at_transaction*>(data.data());
      void*                      out;

      // Try to allocate a buffer to read data in to (plus room for
      // a status code at the front).

      if (reply && reply->setDataSize(rd->size + sizeof(ssize_t)) == B_OK) {
        STUB;  //out = reinterpret_cast<void*>(reply->data());
        amt = rd->size > 0
                  ? ReadAt(rd->position, static_cast<uint8_t*>(out) + sizeof(ssize_t), rd->size)
                  : 0;
        *static_cast<ssize_t*>(out) = amt;
        if (amt >= 0)
          reply->setDataSize(amt + sizeof(ssize_t));
        else
          reply->setDataSize(sizeof(ssize_t));
      }
      else {
        amt = B_NO_MEMORY;
      }
    }
    return amt >= B_OK ? B_OK : amt;
  }
  else if (code == B_WRITE_AT_TRANSACTION) {
    const write_at_transaction* wr = NULL;  //STUB
    STUB;                                   //static_cast<const write_at_transaction*>(data.Data());
    iovec iov;
    iov.iov_base   = (void*)(wr + 1);
    iov.iov_len    = data.dataSize();
    ssize_t result = NULL;  // STUB
    STUB;                   //B_BINDER_BAD_TRANSACT;
    if (wr && iov.iov_len >= sizeof(*wr)) {
      iov.iov_len -= sizeof(wr);
      result = WriteAtV(wr->position, &iov, 1);
    }
    if (reply) reply->setData(reinterpret_cast<const uint8_t*>(&result), sizeof(result));
    return result >= B_OK ? B_OK : result;
  }
  else {
    return BBinder::transact(code, data, reply, flags);
  }
}

static SValue
storage_hook_Size(const sptr<IInterface>& i)
{
  STUB;
  return SValue();  //STUB: SValue::Offset(static_cast<IStorage*>(i.ptr())->Size());
}

static SValue
storage_hook_SetSize(const sptr<IInterface>& This, const SValue& args)
{
  SValue   param;
  off_t    size;
  status_t error;

  STUB;
  return param;  //STUB
  /*if (!(param = args[SValue::Int32(0)]).IsDefined()) return SValue::SSize(B_BINDER_MISSING_ARG);
  size = param.AsOffset(&error);
  if (error != B_OK) return SValue::SSize(B_BINDER_BAD_TYPE);

  return SValue::SSize(static_cast<IStorage*>(This.ptr())->SetSize(size));*/
}

static SValue
storage_hook_ReadAt(const sptr<IInterface>& This, const SValue& args)
{
  SValue   param;
  status_t error;

  off_t position;
  iovec iov;

  STUB; /*if (!(param = args[SValue::Int32(0)]).IsDefined()) return SValue::SSize(B_BINDER_MISSING_ARG);
  position = param.AsOffset(&error);
  if (error != B_OK) return SValue::SSize(B_BINDER_BAD_TYPE);

  if (!(param = args[SValue::Int32(1)]).IsDefined()) return SValue::SSize(B_BINDER_MISSING_ARG);
  iov.iov_len = param.AsInt32(&error);
  if (error != B_OK) return SValue::SSize(B_BINDER_BAD_TYPE);*/

  SValue result;
  STUB; /*iov.iov_base = result.BeginEditBytes(B_RAW_TYPE, iov.iov_len);
  if (iov.iov_base) {
    ssize_t s = static_cast<IStorage*>(This.ptr())->ReadAtV(position, &iov, 1);
    result.EndEditBytes(s);
    if (s < B_OK) result = SValue::SSize(s);
  }
  else {
    result = SValue::SSize(B_NO_MEMORY);
  }*/

  return result;
}

static SValue
storage_hook_WriteAt(const sptr<IInterface>& This, const SValue& args)
{
  SValue   param;
  status_t error;

  off_t position;

  return param;  //STUB
  STUB;          /*if (!(param = args[SValue::Int32(0)]).IsDefined()) return SValue::SSize(B_BINDER_MISSING_ARG);
  position = param.AsOffset(&error);
  if (error != B_OK) return SValue::SSize(B_BINDER_BAD_TYPE);

  if (!(param = args[SValue::Int32(1)]).IsDefined()) return SValue::SSize(B_BINDER_MISSING_ARG);
  if (!param.IsSimple() && param.Data()) return SValue::SSize(B_BINDER_BAD_TYPE);

  return SValue::SSize(static_cast<IStorage*>(This.ptr())
                           ->WriteAt(position, param.Data(), param.Length()));*/
}

static SValue
storage_hook_Sync(const sptr<IInterface>& This, const SValue& args)
{
  return args;  //STUB
  STUB;
  //return SValue::SSize(static_cast<IStorage*>(This.ptr())->Sync());
}

//static const struct effect_action_def storage_actions[] = {
//    {sizeof(effect_action_def), &key_Size,
//     NULL, NULL, storage_hook_Size, NULL},
//    {sizeof(effect_action_def), &key_SetSize,
//     NULL, NULL, NULL, storage_hook_SetSize},
//    {sizeof(effect_action_def), &key_ReadAt,
//     NULL, NULL, NULL, storage_hook_ReadAt},
//    {sizeof(effect_action_def), &key_WriteAt,
//     NULL, NULL, NULL, storage_hook_WriteAt},
//    {sizeof(effect_action_def), &key_Sync,
//     NULL, NULL, NULL, storage_hook_Sync}};

status_t
BnStorage::HandleEffect(const SValue& in, const SValue& inBindings, const SValue& outBindings, SValue* out)
{
  STUB;
  return 0; /*STUB: execute_effect(sptr<IInterface>(this),
                        in, inBindings, outBindings, out,
                        storage_actions, sizeof(storage_actions) / sizeof(storage_actions[0]));*/
}

}  // namespace support
}  // namespace os
