<?php
/**
 * @file
 * Rules module integration
 */

/**
 * Implements hook_event_info().
 */
function twilio_rules_event_info() {
  $defaults = array(
    'group' => t('Twilio'),
    'module' => 'twilio',
  );
  return array(
    'twilio_sms_incoming' => $defaults + array(
      'label' => t('Drupal receives an SMS'),
      'variables' => array(
        'number' => array(
          'type' => 'text',
          'label' => t('Number'),
        ),
        'message' => array(
          'type' => 'text',
          'label' => t('Message'),
        ),
      ),
    ),
    'twilio_voice_incoming' => $defaults + array(
      'label' => t('Drupal receives a voice call'),
      'variables' => array(
        'number' => array(
          'type' => 'text',
          'label' => t('Number'),
        ),
      ),
    ),
    'twilio_sms_incoming_from_user' => $defaults + array(
      'label' => t('Drupal receives an SMS from a site user'),
      'variables' => array(
        'number' => array(
          'type' => 'text',
          'label' => t('Number'),
        ),
        'message' => array(
          'type' => 'text',
          'label' => t('Message'),
        ),
        'account' => array(
          'type' => 'user',
          'label' => t('User'),
          'save' => TRUE,
        ),
      ),
    ),
  );
}

/**
 * Implements hook_rules_condition_info().
 */
function twilio_rules_condition_info() {
  return array(
    'twilio_rules_condition_number_belongs_to_user' => array(
      'label' => t('Number belongs to a valid user'),
      'arguments' => array(
        'number' => array(
          'type' => 'text',
          'label' => t('Phone Number'),
        ),
      ),
      'group' => t('Twilio'),
      'module' => 'twilio',
    ),
  );
}

/**
 * Implements hook_rules_action_info().
 */
function twilio_rules_action_info() {
  $defaults = array(
    'group' => t('Twilio'),
    'module' => 'twilio',
  );
  $actions['twilio_send_sms_to_user'] = $defaults + array(
    'label' => t('Send an SMS to a user'),
    'base' => 'twilio_rules_action_send_sms_to_user',
    'parameter' => array(
      'account' => array(
        'type' => 'user',
        'label' => t('User'),
        'save' => TRUE,
      ),
      'message' => array(
        'type' => 'text',
        'label' => t('Message'),
      ),
    ),
  );
  $actions['twilio_twiml_output'] = $defaults + array(
    'label' => t('Output TwiML'),
    'base' => 'twilio_rules_twiml_output',
    'parameter' => array(
      'twiml_id' => array(
        'type' => 'text',
        'label' => t('TwiML'),
        'options list' => 'twilio_twiml_options_list',
        'restriction' => 'input',
      ),
    ),
  );
  $actions['twilio_send_sms_to_number'] = $defaults + array(
    'label' => t('Send an SMS to a number'),
    'base' => 'twilio_rules_action_send_sms_to_number',
    'parameter' => array(
      'country' => array(
        'type' => 'text',
        'label' => t('Country code'),
        'options list' => 'twilio_country_codes',
      ),
      'number' => array(
        'type' => 'text',
        'label' => t('Number'),
      ),
      'message' => array(
        'type' => 'text',
        'label' => t('Message'),
      ),
    ),
  );
  return $actions;
}

/**
 * Action: Send an SMS to a user.
 */
function twilio_rules_action_send_sms_to_user($account, $message) {
  if (!empty($account->twilio_user['number']) && $account->twilio_user['status'] == 2) {
    twilio_send($account->twilio_user['number'], $message, $account->twilio_user['country']);
  }
}

/**
 * Action: Send an SMS to a number.
 */
function twilio_rules_action_send_sms_to_number($country, $number, $message) {
  twilio_send($number, $message, $country);
}

/**
 * Condition: Validate if a number belongs to a user on the site.
 */
function twilio_rules_condition_number_belongs_to_user($number) {
  return twilio_verify_number($number);
}

/**
 * Action: Output TwiML.
 */
function twilio_rules_twiml_output($twiml_id) {
  $twiml = twilio_twiml_load($twiml_id);
  echo $twiml['data'];
  exit();
}
