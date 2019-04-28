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

#ifndef PACKAGE_MANIFESTPARSER_H
#define PACKAGE_MANIFESTPARSER_H

#include <support/Atom.h>
#include <support/ByteStream.h>
#include <support/Package.h>
#include <support/String.h>

namespace os {
namespace package {

class SManifestParser : public support::SAtom
{
 public:
  //!	Parses an IByteInput and calls the callbacks on a parser
  static status_t ParseManifest(const support::SString& filename, const support::sptr<SManifestParser>& parser, const support::sptr<support::IByteInput>& stream, const support::SPackage& resources, const support::SString& package);

  //!	Parses an IByteInput that is at the beginning of a package file, looking for an
  //	embedded manifest file, and calls the callbacks on a parser
  static status_t ParseManifestFromPackageFile(const support::SString& filename, const support::sptr<SManifestParser>& parser, const support::sptr<support::IByteInput>& stream, const support::SPackage& resources, const support::SString& package);

  //! Callback: A protein plugin was declared
  virtual void OnDeclareAddon(const support::SValue& addonInfo) = 0;

  //! Callback: An application was declared
  virtual void OnDeclareApplication(const support::SValue& appInfo) = 0;

  //! Callback: A new component was declared
  virtual void OnDeclareComponent(const support::SValue& componentInfo) = 0;
};

}  // namespace package
}  // namespace os

#endif /* PACKAGE_MANIFESTPARSER_H */
