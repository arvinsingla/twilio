
--------------------------------------------------------------------------------
                                 Twilio
--------------------------------------------------------------------------------

Maintainers:
 * Arvin Singla (arvinsingla), arvin.singla@gmail.com

The Twilio module provides integration with the Twilio cloud communication
platform allowing for your Drupal site to integration Voice and SMS
functionality. 

The module currently provides the following functionality:

- Rules event to act on incoming SMS message
- Rules action to send an SMS message
- Hook for incoming SMS messages
- Hook for incoming voice calls
- User object phone number storage and validation
- Uses libraries api to manage the Twilio php library

Project homepage: http://drupal.org/project/twilio


Dependencies
------------

* Libraries API - http://www.drupal.org/project/libraries

The Rules module (http://www.drupal.org/project/rules) is required to use the
rules integration.


Installation
------------

1. Install the Twilio module as per (http://drupal.org/node/895232/)
2. Download the Twilio php library from (http://www.twilio.com/docs/libraries).
3. Extract the library in your sites/all/libraries folder and rename the
   directory to 'twilio'.
4. Visit 'admin/config/system/twilio' and enter your Twilio API information
   found on the twilio dashboard 


Documentation
-------------
* Documentation is currently being written and will be posted on d.o soon.
