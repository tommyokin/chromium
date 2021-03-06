// Copyright (c) 2012 The Chromium Authors. All rights reserved.
// Use of this source code is governed by a BSD-style license that can be
// found in the LICENSE file.

#ifndef CHROME_BROWSER_UI_COCOA_SSL_CLIENT_CERTIFICATE_SELECTOR_COCOA_H_
#define CHROME_BROWSER_UI_COCOA_SSL_CLIENT_CERTIFICATE_SELECTOR_COCOA_H_

#import <Cocoa/Cocoa.h>
#include <vector>

#include "base/mac/scoped_cftyperef.h"
#include "base/mac/scoped_nsobject.h"
#include "base/memory/ref_counted.h"
#include "base/memory/scoped_ptr.h"
#include "chrome/browser/ssl/ssl_client_certificate_selector.h"
#import "chrome/browser/ui/cocoa/constrained_window/constrained_window_custom_sheet.h"
#import "chrome/browser/ui/cocoa/constrained_window/constrained_window_sheet_controller.h"

class ConstrainedWindowMac;
@class SFChooseIdentityPanel;
class SSLClientAuthObserverCocoaBridge;

@interface SSLClientCertificateSelectorCocoa
    : NSObject<ConstrainedWindowSheet> {
 @private
  // The list of identities offered to the user.
  base::ScopedCFTypeRef<CFMutableArrayRef> identities_;
  // The corresponding list of certificates.
  std::vector<scoped_refptr<net::X509Certificate> > certificates_;
  // A C++ object to bridge SSLClientAuthObserver notifications to us.
  scoped_ptr<SSLClientAuthObserverCocoaBridge> observer_;
  base::scoped_nsobject<SFChooseIdentityPanel> panel_;
  scoped_ptr<ConstrainedWindowMac> constrainedWindow_;
  base::scoped_nsobject<NSWindow> overlayWindow_;
  BOOL closePending_;
  // A copy of the sheet's frame used to restore on show.
  NSRect oldSheetFrame_;
  // A copy of the sheet's |autoresizesSubviews| flag to restore on show.
  BOOL oldResizesSubviews_;
}

@property (readonly, nonatomic) SFChooseIdentityPanel* panel;

- (id)initWithNetworkSession:(const net::HttpNetworkSession*)networkSession
             certRequestInfo:(net::SSLCertRequestInfo*)certRequestInfo
                    callback:(const chrome::SelectCertificateCallback&)callback;
- (void)displayForWebContents:(content::WebContents*)webContents;
- (void)closeWebContentsModalDialog;

- (NSWindow*)overlayWindow;

@end

#endif  // CHROME_BROWSER_UI_COCOA_SSL_CLIENT_CERTIFICATE_SELECTOR_COCOA_H_
