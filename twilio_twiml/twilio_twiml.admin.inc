<?php
/**
 * @file
 * Administrative forms section
 */

/**
 * TwiML Manager form.
 */
function twilio_twiml_manager_form($form, &$form_state, $twiml_id = NULL) {

  // Load the TwiML data if it exists.
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

  // If the PHP module is enabled show a different description.
  if (module_exists('php')) {
    $description = t('The TwiML. You may use PHP code. You should include &lt;?php ?&gt; tags.');
  }
  else {
    $description = t('The TwiML. If you wish to use PHP code you must enable the !phpmodule', array('!phpmodule' => l(t('PHP filter module'), 'admin/modules')));
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
  elseif ($form_state['triggering_element']['#name'] == 'update') {
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
