<?php
/**
 * @file
 * User facing pages
 */

/**
 * Menu callback for adding and confirming a user's mobile number.
 */
function twilio_user_settings($account) {
  return drupal_get_form('twilio_user_settings_form', $account);
}

/**
 * User settings form.
 */
function twilio_user_settings_form($form, &$form_state, $account) {

  if (isset($form_state['values']['account'])) {
    $account = user_load($form_state['values']['account']->uid);
  }

  $form['#prefix'] = '<div id="twilio-user-form">';
  $form['#suffix'] = '</div>';

  $form['uid'] = array(
    '#type' => 'hidden',
    '#value' => $account->uid,
  );

  if (empty($account->twilio_user['status'])) {
    $form['countrycode'] = array(
      '#type' => 'select',
      '#title' => t('Country code'),
      '#options' => twilio_country_codes(),
    );
    $form['number'] = array(
      '#type' => 'textfield',
      '#title' => t('Phone number'),
      '#description' => t('A confirmation code will be sent to via SMS to the number provided'),
      '#size' => 40,
      '#maxlength' => 255,
      '#required' => TRUE,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Confirm number'),
      '#validate' => array('twilio_user_settings_add_form_validate'),
      '#submit' => array('twilio_user_settings_add_form_submit'),
      '#ajax' => array(
        'callback' => 'twilio_user_settings_ajax_callback',
        'wrapper' => 'twilio-user-form',
        'method' => 'replace',
      ),
    );
  }
  elseif ($account->twilio_user['status'] == 1) {
    $form['number'] = array(
      '#type' => 'item',
      '#title' => t('Mobile phone number'),
      '#markup' => $account->twilio_user['number'],
    );
    $form['confirm_code'] = array(
      '#type' => 'textfield',
      '#title' => t('Confirmation code'),
      '#description' => t('Enter the confirmation code sent by SMS to your mobile phone.'),
      '#size' => 4,
      '#maxlength' => 4,
    );
    $form['submit'] = array(
      '#type' => 'submit',
      '#value' => t('Confirm number'),
      '#validate' => array('twilio_user_settings_confirm_form_validate'),
      '#submit' => array('twilio_user_settings_confirm_form_submit'),
      '#ajax' => array(
        'callback' => 'twilio_user_settings_ajax_callback',
        'wrapper' => 'twilio-user-form',
        'method' => 'replace',
      ),
    );
    $form['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Delete & start over'),
      '#validate' => array('twilio_user_settings_confirm_form_validate'),
      '#submit' => array('twilio_user_settings_confirm_form_submit'),
      '#ajax' => array(
        'callback' => 'twilio_user_settings_ajax_callback',
        'wrapper' => 'twilio-user-form',
        'method' => 'replace',
      ),
    );
  }
  elseif ($account->twilio_user['status'] == 2) {
    $form['twilio_user']['number'] = array(
      '#type' => 'item',
      '#title' => t('Your mobile phone number'),
      '#markup' => '+' . $account->twilio_user['country'] . ' ' . $account->twilio_user['number'],
      '#description' => t('Your mobile phone number has been confirmed.'),
    );
    $form['reset'] = array(
      '#type' => 'submit',
      '#value' => t('Delete & start over'),
      '#validate' => array('twilio_user_settings_reset_form_validate'),
      '#submit' => array('twilio_user_settings_reset_form_submit'),
      '#ajax' => array(
        'callback' => 'twilio_user_settings_ajax_callback',
        'wrapper' => 'twilio-user-form',
        'method' => 'replace',
      ),
    );
  }
  return $form;
}

/**
 * Ajax callback for user forms.
 */
function twilio_user_settings_ajax_callback($form, &$form_state) {
  return $form;
}

/**
 * Validation function for user settings form.
 *
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function twilio_user_settings_add_form_validate($form, &$form_state) {
  $value = $form_state['values']['number'];
  if (!is_numeric($value)) {
    form_set_error('number', t('You must enter a valid phone number'));
  }
  elseif (twilio_verify_duplicate_number($form_state['values']['number'])) {
    form_set_error('number', t('This number is already in use and cannot be assigned to more than one account'));
  }
}

/**
 * Submit handler for user settings form.
 *
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function twilio_user_settings_add_form_submit($form, &$form_state, $account = NULL) {
  if (!$account) {
    $account = user_load($form_state['values']['uid']);
  }
  $account = twilio_user_send_confirmation($account, $form_state['values']['number'], $form_state['values']['countrycode']);
  $form_state['values']['account'] = $account;
  $form_state['rebuild'] = TRUE;
  drupal_set_message(t("A confirmation code has been sent to your mobile device"), 'status');
}

/**
 * Validation handler for user settings confirmation form.
 *
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function twilio_user_settings_confirm_form_validate($form, &$form_state) {
  if ($form_state['clicked_button']['#value'] == t('Confirm number')) {
    $account = user_load($form_state['values']['uid']);
    if ($form_state['values']['confirm_code'] != $account->twilio_user['code']) {
      form_set_error('confirm_code', t('The confirmation code is invalid.'));
    }
  }
}

/**
 * Submit handler for user settings confirmation form.
 *
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function twilio_user_settings_confirm_form_submit($form, &$form_state) {
  $account = user_load($form_state['values']['uid']);
  if ($form_state['clicked_button']['#value'] == t('Delete & start over')) {
    twilio_user_delete($account);
    $account = user_load($form_state['values']['uid'], TRUE);
    drupal_set_message(t('Your mobile information has been removed'), 'status');
  }
  else {
    $data = array(
      'number' => $account->twilio_user['number'],
      'status' => TWILIO_USER_CONFIRMED,
    );
    $account = user_save($account, array('twilio' => $data), 'twilio');
    drupal_set_message(t('Your mobile number has been confirmed'), 'status');
  }
  $form_state['values']['account'] = $account;
  $form_state['rebuild'] = TRUE;
}

/**
 * Validation handler for user settings reset form.
 */
function twilio_user_settings_reset_form_validate($form, &$form_state) {
}

/**
 * Submit handler for user settings reset form.
 *
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function twilio_user_settings_reset_form_submit($form, &$form_state) {
  $account = user_load($form_state['values']['uid']);
  twilio_user_delete($account);
  $account = user_load($form_state['values']['uid'], TRUE);
  drupal_set_message(t('Your mobile information has been removed'), 'status');
  $form_state['values']['account'] = $account;
  $form_state['rebuild'] = TRUE;
}
