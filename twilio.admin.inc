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
  $form['twilio_long_sms'] = array(
    '#type' => 'radios',
    '#title' => t('Long SMS handling'),
    '#description' => t('How would you like to handle SMS messages longer than 160 characters.'),
    '#options' => array(t('Send multiple messages'), t('Truncate message to 160 characters')),
    '#default_value' => variable_get('twilio_long_sms', TWILIO_SMS_LONG_MULTIPLE),
  );
  $form['twilio_registration_form'] = array(
    '#type' => 'radios',
    '#title' => t('Show mobile fields during user registration'),
    '#description' => t('Specify if the site should collect mobile information during registration.'),
    '#options' => array(t('Disabled'), t('Optional'), t('Required')),
    '#default_value' => variable_get('twilio_registration_form', 0),
  );

  // Expose the callback URLs to the user for convenience
  $form['twilio_callback_container'] = array(
    '#type' => 'fieldset', 
    '#title' => t('Module callbacks'),
    '#description' => t('Enter these callback addresses into your Twilio phone number configuration on the Twilio dashboard to allow your site to respond to incoming voice calls and SMS messages.'),
  );
  
  // Initialize URL variables
  $voice_callback = $GLOBALS['base_url'] . '/twilio/voice';
  $sms_callback = $GLOBALS['base_url'] . '/twilio/sms';
  
  $form['twilio_callback_container']['voice_callback'] = array(
    '#type' => 'item',
    '#title' => t('Voice request URL'),
    '#markup' => '<p>' . $voice_callback . '</p>',
  );
  
  $form['twilio_callback_container']['sms_callback'] = array(
    '#type' => 'item',
    '#title' => t('SMS request URL'),
    '#markup' => '<p>' . $sms_callback . '</p>',
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
 * TwiML Manager form.
 */
function twilio_twiml_manager_form($form, &$form_state, $twiml_id = NULL) {

  // Load the TwiML data if it exists
  $twiml = twilio_twiml_load($twiml_id);
  $twiml_list = twilio_twiml_load_multiple();

  $form = array();

  if (!empty($twiml_list)) {
    $form['list'] = array(
      '#theme' => 'twilio_twiml_list',
      '#twiml_list' => $twiml_list,
    );
  }

  $form['name'] = array(
    '#title' => t('Name'),
    '#type' => 'textfield',
    '#default_value' => $twiml['name'],
    '#description' => t('The human-readable name of this TwiML.'),
    '#required' => TRUE,
    '#size' => 30,
  );

  $form['twiml_id'] = array(
    '#type' => 'machine_name',
    '#default_value' => $twiml['twiml_id'],
    '#maxlength' => 32,
    '#disabled' => $twiml,
    '#machine_name' => array(
      'exists' => 'twilio_twiml_load',
    ),
    '#description' => t('A unique machine-readable name for this TwiML. It must only contain lowercase letters, numbers, and underscores.'),
  );

  $form['description'] = array(
    '#title' => t('Description'),
    '#type' => 'textfield',
    '#default_value' => $twiml['description'],
    '#description' => t('The description of this TwiML.'),
  );

  // If the PHP module is enabled show a different description
  if (module_exists('php')) {
    $description = t('The TwiML. You may use PHP code. You should include &lt;?php ?&gt; tags.');
  }
  else {
    $description= t('The TwiML. If you wish to use PHP code you must enable the !phpmodule', array('!phpmodule' => l('PHP filter module', 'admin/modules')));
  }
  
  $form['data'] = array(
    '#title' => t('TwiML'),
    '#type' => 'textarea',
    '#default_value' => $twiml['data'],
    '#description' => $description,
  );

  $form['save'] = array(
    '#value' => $twiml ? t('Update TwiML') : t('Create TwiML'),
    '#type' => 'submit',
    '#name' => $twiml ? 'update' : 'save',
  );

  return $form;
}

/**
 * Submit callback for TwiML Manager form.
 */
function twilio_twiml_manager_form_submit($form, &$form_state) {
  // Prep vars.
  $values = $form_state['values'];
  // Create entry in DB.
  if ($form_state['triggering_element']['#name'] == 'save') {
    twilio_twiml_create($values);
  }
  // Over write entry in DB.
  else if ($form_state['triggering_element']['#name'] == 'update') {
    twilio_twiml_update($values);
  }
}

/**
 * Form constructor for the TwiML deletion confirmation form.
 */
function twilio_twiml_delete_confirm($form, &$form_state, $twiml = FALSE) {
  $form['twiml'] = array('#type' => 'value', '#value' => $twiml);
  return confirm_form($form,
    t('Are you sure you want to delete %title?', array('%title' => $twiml['name'])),
    TWILIO_ADMIN_PATH . '/twiml',
    t('This is a permanent action.'),
    t('Delete'),
    t('Cancel')
  );
}

/**
 * Executes TwiML deletion.
 */
function twilio_twiml_delete_confirm_submit($form, &$form_state) {
  if ($form_state['values']['confirm']) {
    twilio_twiml_delete($form_state['values']['twiml']['twiml_id']);
    drupal_set_message(t('%title has been deleted.', array('%title' => $form_state['values']['twiml']['name'])));
  }

  $form_state['redirect'] = TWILIO_ADMIN_PATH . '/twiml';
}
