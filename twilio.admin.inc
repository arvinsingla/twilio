<?php
/**
 * @file
 * Administrative forms section
 */

/**
 * Administration form for twilio settings.
 */
function twilio_admin_form($form, &$form_state) {

  $library = libraries_detect(TWILIO_LIBRARY);
  if (empty($library['installed'])) {
    $twilio_library_status = t('The Twilo library is not currently installed');
  }
  else {
    $twilio_library_status = t('The Twilio library is installed correctly');
  }

  $form['twilio_library_status'] = array(
    '#type' => 'markup',
    '#markup' => $twilio_library_status,
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
    '#element_validate' => array('twilio_admin_phone_validate'),
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
 * Element validator for the phone number field.
 */
function twilio_admin_phone_validate($element, &$form_state, $form) {
  if ($number = twilio_validate_number($element['#value'], FALSE)) {
    $form_state['values']['twilio_number'] = $number;
  }
  else {
    form_error($element, t('You must enter a valid 10 digit phone number'));
  }
}

/**
 * Administrative testing form for SMS.
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
 * Validation handler for the administrative test message form.
 */
function twilio_admin_test_form_validate($form, &$form_state) {
  $number = twilio_validate_number($form_state['values']['number']);
  if ($number) {
    $form_state['values']['number'] = $number;
  }
  else {
    form_set_error('number', t('You must enter a valid 10 digit phone number'));
  }
}

/**
 * Submit handler for the administrative test message testing form.
 */
function twilio_admin_test_form_submit($form, &$form_state) {
  twilio_send($form_state['values']['number'], $form_state['values']['message']);
  drupal_set_message(t('Your message has been sent'));
}
