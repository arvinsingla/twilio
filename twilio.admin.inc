<?php

/**
 * Administration form for twilio settings
 */
function twilio_admin_form($form, &$form_state) {
  $form['twilio_path'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Twilio Path'),
    '#default_value' => variable_get('twilio_path'),
    '#description' => t('Enter the full path to the twilio library. e.g. sites/all/libraries/twilio/Services/Twilio.php'),
  );
  $form['twilio_account'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Twilio Account ID'),
    '#default_value' => variable_get('twilio_account'),
    '#description' => t('Enter your Twilio account id'),
  );
  $form['twilio_token'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Twilio token ID'),
    '#default_value' => variable_get('twilio_token'),
    '#description' => t('Enter your Twilio token id'),
  );
  $form['twilio_number'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Twilio Phone Number'),
    '#default_value' => variable_get('twilio_number'),
    '#description' => t('Enter your Twilio phone number'),
  );
  $form['twilio_registration_form'] = array(
    '#type' => 'radios',
    '#title' => t('Show mobile fields during user registration'),
    '#description' => t('Specify if the site should collect mobile information during registration.'),
    '#options' => array(t('Disabled'), t('Optional'), t('Required')),
    '#default_value' => variable_get('twilio_registration_form', 0),
  );
  return system_settings_form($form); 
}

/**
 * Administrative testing form for SMS
 */
function twilio_admin_test_form($form, &$form_state) {
  $form['number'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => 'Phone Number',
  );
  $form['message'] = array(
    '#type' => 'textarea',
    '#required'=> TRUE,
    '#title' => 'Message',
  );
  $form['submit'] = array(
    '#type' => 'submit',
    '#value' => t('Send SMS'),
  );
  return $form;
}

/**
 * Validation handler for the administrative test message form
 */
function twilio_admin_test_form_validate($form, &$form_state) {
  $number = $form_state['values']['number'];
  if(!twilio_validate_number($number)){
    form_set_error('number', t('You must enter a valid 10 digit phone number'));
  }
}

/**
 * Submit handler for the administrative test message testing form
 */
function twilio_admin_test_form_submit($form, &$form_state) {
  twilio_send($form_state['values']['number'], $form_state['values']['message']);
  drupal_set_message(t('Your message has been sent'));
}