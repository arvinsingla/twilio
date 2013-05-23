<?php
/**
 * @file
 * Administrative forms section
 */

/**
 * Administration form for twilio settings.
 */
function twilio_admin_form($form, &$form_state) {

  // Detect the Twilio library and provide feedback to the user if not present.
  if (module_exists('libraries') && function_exists('libraries_detect')) {
      $library = libraries_detect(TWILIO_LIBRARY);
      if (!$library['installed']) {
        $twilio_readme_link = l('README.txt', 'http://drupalcode.org/project/twilio.git/blob/refs/heads/7.x-1.x:/README.txt' );
        $twilio_error_text = t('The Twilo library was not detected or installed correctly. Please review the installation instructions provided in the !link file', array('!link' => $twilio_readme_link));
        drupal_set_message($twilio_error_text, 'error');
      }
  }

  $form['twilio_account'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Twilio Account SID'),
    '#default_value' => variable_get('twilio_account'),
    '#description' => t('Enter your Twilio account id'),
  );
  $form['twilio_token'] = array(
    '#type' => 'textfield',
    '#required' => TRUE,
    '#title' => t('Twilio Auth Token'),
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

/**
 * TwML Manager form.
 */
function twilio_twml_manager_form($form, &$form_state, $twml_id = NULL) {

  // Load the TwML data if it exists
  $twml = twilio_twml_load($twml_id);
  $twml_list = twilio_twml_load_multiple();

  $form = array();

  if (!empty($twml_list)) {
    $form['list'] = array(
      '#theme' => 'twilio_twml_list',
      '#twml_list' => $twml_list,
    );
  }

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $twml['name'],
    '#description' => t('The human-readable name of this TwML.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['twml_id'] = array(
    '#type' => 'machine_name',
    '#default_value' => $twml['twml_id'],
    '#maxlength' => 32,
    '#disabled' => $twml,
    '#machine_name' => array(
      'exists' => 'twilio_twml_load',
    ),
    '#description' => t('A unique machine-readable name for this TwML. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#default_value' => $twml['description'],
    '#description' => t('The description of this TwML.'),
  );

  $form['data'] = array(
    '#title' => t('TwML'),
    '#type' => 'textarea',
    '#default_value' => $twml['data'],
    '#description' => t('The TwML data.'),
  );

  $form['save'] = array(
    '#value' => $twml ? t('Update TwML') : t('Create TwML'),
    '#type' => 'submit',
    '#name' => $twml ? 'update' : 'save',
  );

  $form['cancel'] = array(
    '#value' => t('Cancel'),
    '#type' => 'submit',
    '#name' => 'cancel',
  );

  return $form;
}

/**
 * Submit callback for TwML Manager form.
 */
function twilio_twml_manager_form_submit($form, &$form_state) {
  // Prep vars.
  $values = $form_state['values'];
  // Create entry in DB.
  if ($form_state['triggering_element']['#name'] == 'save') {
    twilio_twml_create($values);
  }
  // Over write entry in DB.
  else if ($form_state['triggering_element']['#name'] == 'update') {
    twilio_twml_update($values);
  }
  // Cancel... just start over.
  else if ($form_state['triggering_element']['#name'] == 'cancel') {
    drupal_goto(TWILIO_ADMIN_PATH . '/twml');
  }
}

/**
 * Form constructor for the TwML deletion confirmation form.
 */
function twilio_twml_delete_confirm($form, &$form_state, $twml = FALSE) {
  $form['twml'] = array('#type' => 'value', '#value' => $twml);
  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $twml['name'])),
    TWILIO_ADMIN_PATH . '/twml',
    t('This is a permanent action.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Executes TwML deletion.
 */
function twilio_twml_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    twilio_twml_delete($form_state['values']['twml']['twml_id']);
    drupal_set_message(t('%title has been deleted.', array('%title' => $form_state['values']['twml']['name'])));
  }

  $form_state['redirect'] = TWILIO_ADMIN_PATH . '/twml';
}
