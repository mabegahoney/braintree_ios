#import <Foundation/Foundation.h>
#import <BraintreeCore/BTAPIClient.h>
#import <BraintreeCore/BTAppSwitch.h>
#import <BraintreeCore/BTNullability.h>

#import "BTVenmoTokenizedCard.h"

BT_ASSUME_NONNULL_BEGIN

@protocol BTVenmoDriverDelegate;

extern NSString * const BTVenmoDriverErrorDomain;

typedef NS_ENUM(NSInteger, BTVenmoDriverErrorType) {
    BTVenmoDriverErrorTypeUnknown = 0,
    /// Venmo is disabled in configuration
    BTVenmoDriverErrorTypeDisabled,
    /// App is not installed on device
    BTVenmoDriverErrorTypeAppNotAvailable,
    /// Bundle display name must be present
    BTVenmoDriverErrorTypeBundleDisplayNameMissing,
    /// UIApplication failed to switch to Venmo app
    BTVenmoDriverErrorTypeAppSwitchFailed,
    /// Return URL was invalid
    BTVenmoDriverErrorTypeInvalidReturnURL,
};

@interface BTVenmoDriver : NSObject <BTAppSwitchHandler>

/// Initialize a new Venmo driver instance.
///
/// @param apiClient The API client
- (instancetype)initWithAPIClient:(BTAPIClient *)apiClient NS_DESIGNATED_INITIALIZER;


- (instancetype)init __attribute__((unavailable("Please use initWithAPIClient:")));


/// Initiates Venmo login via app switch, which returns a tokenized card when successful.
///
/// @note If the BTAPIClient was initialized with a JWT, the tokenizedCard will have the card
/// network and the last 2 digits of the card number. With a client key, these properties will be `nil`.
///
/// @param completionBlock This completion will be invoked when app switch is complete or an error occurs.
/// On success, you will receive an instance of `BTVenmoTokenizedCard`; on failure, an error; on user
/// cancellation, you will receive `nil` for both parameters.
- (void)tokenizeVenmoCardWithCompletion:(void (^)(BTVenmoTokenizedCard * __BT_NULLABLE tokenizedCard, NSError * __BT_NULLABLE error))completionBlock;

/// Indicates whether the Venmo app is available for app switch. This should be checked before presenting
/// any UI to pay with Venmo.
///
/// @note This only indicates if the app is installed and available to be launched, not whether Venmo is
/// enabled for your merchant account.
@property (nonatomic, readonly, assign) BOOL isAppSwitchAvailable;

/// The delegate for `BTVenmoDriver`.
@property (nonatomic, weak) id<BTVenmoDriverDelegate> delegate;

@end

/// A delegate protocol for sending lifecycle updates as Venmo login via app switch takes place
@protocol BTVenmoDriverDelegate <NSObject>

@optional

/// Delegates receive this message when the Venmo driver is preparing to perform an app switch.
///
/// This transition is usually instantaneous; however, you may use this hook to present a loading
/// indication to the user.
///
/// @param venmoDriver The BTVenmoDriver instance performing user authentication
- (void)venmoDriverWillPerformAppSwitch:(BTVenmoDriver *)venmoDriver;

/// Delegates receive this message when the Venmo driver has successfully performed an app switch.
///
/// You may use this hook to prepare your UI for app switch return. Keep in mind that
/// users may manually switch back to your app via the iOS task manager.
///
/// @note You may also hook into the app switch lifecycle via UIApplicationWillResignActiveNotification.
///
/// @param venmoDriver The BTVenmoDriver instance performing user authentication
- (void)venmoDriverDidPerformAppSwitch:(BTVenmoDriver *)venmoDriver;

/// Delegates receive this message when control returns to BTVenmoDriver upon app switch return
///
/// This usually gets invoked after handleAppSwitchReturnURL: is called in your UIApplicationDelegate.
///
/// @note You may also hook into the app switch lifecycle via UIApplicationWillResignActiveNotification.
///
/// @param venmoDriver The instance of BTVenmoDriver handling the app switch return.
- (void)venmoDriverWillProcessAppSwitchReturn:(BTVenmoDriver *)venmoDriver;

@end


BT_ASSUME_NONNULL_END