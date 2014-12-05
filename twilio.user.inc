<?php

/**
 * @file
 * Twilio user account and registration related functionality
 */

/**
 * Implements hook_user_categories().
 */
function twilio_user_categories() {
  return array(
    array(
      'name' => 'twilio',
      'title' => t('Mobile SMS'),
      'weight' => 3,
    ),
  );
}

/**
 * Implements hook_user_load().
 */
function twilio_user_load($users) {
  // Load data from the {twilio_user} table for the user account.
  $result = db_select('twilio_user', 'u')
    ->fields('u', array('uid', 'country', 'number', 'status', 'code'))
    ->condition('uid', array_keys($users), 'IN')
    ->execute();
  foreach ($result as $record) {
    if (!empty($record->uid)) {
      // Assign the twilio data to the user object.
      $users[$record->uid]->twilio_user = (array) $record;
    }
  }
}

/**
 * Implements hook_user_insert().
 */
function twilio_user_insert(&$edit, $account, $category) {
  twilio_user_save($edit, $account, $category);
}

/**
 * Implements hook_user_update().
 */
function twilio_user_update(&$edit, $account, $category) {
  twilio_user_save($edit, $account, $category);
}

/**
 * Implements hook_user_delete().
 */
function twilio_user_delete($account) {
  db_delete('twilio_user')
    ->condition('uid', $account->uid)
    ->execute();
}

/**
 * Saves mobile number data to the {twilio_user} table in the database.
 */
function twilio_user_save(&$edit, $account, $category) {
  if (($category == 'twilio') && isset($edit['twilio'])) {
    $number = (object) $edit['twilio'];
    $number->uid = $account->uid;

    $primary_keys = array();
    if (isset($account->twilio_user['status'])) {
      if ($account->twilio_user['status'] == TWILIO_USER_PENDING && $edit['twilio']['status'] == TWILIO_USER_CONFIRMED) {
        $primary_keys = array('uid');
      }
    }
    drupal_write_record('twilio_user', $number, $primary_keys);

    $edit['twilio']['number'] = NULL;
    $edit['twilio']['status'] = NULL;
    $edit['twilio']['code'] = NULL;
  }
}

/**
 * Implements hook_user_login().
 */
function twilio_user_login(&$edit, $account) {
  // If the users mobile number is in the verification state let them know they
  // need to enter their verification code and link to their settings page.
  if (twilio_edit_access($account) && !empty($account->twilio_user) && $account->twilio_user['status'] == 1) {
    $account_link = l(t("account settings page"), 'user/' . $account->uid . '/edit/twilio');
    drupal_set_message(t("You must confirm your phone number by entering the verification code sent to you via SMS. Go to the !link to enter your verification code.", array('!link' => $account_link)), 'warning');
  }
}


/**
 * Implements hook_FORMID_form_alter().
 */
function twilio_form_user_register_form_alter(&$form, &$form_state, $form_id) {
  if ($twilio_registration_form = variable_get('twilio_registration_form', 0)) {
    if ($twilio_registration_form == 2) {
      $required = TRUE;
    }
    else {
      $required = FALSE;
    }
    $form['account']['countrycode'] = array(
      "#type" => 'select',
      '#options' => twilio_country_codes(),
      '#title' => t('Country code'),
    );
    $form['account']['number'] = array(
      '#type' => 'textfield',
      '#title' => t('Phone number'),
      '#required' => $required,
    );
    $form['#validate'][] = 'twilio_register_validate';
    $form['#submit'][] = 'twilio_register_submit';
  }
}

/**
 * Custom validation function for phone numbers during registration.
 */
function twilio_register_validate($form, &$form_state) {
  $value = $form_state['values']['number'];
  // Phone number is not required and not entered.
  if (empty($value) && empty($form['account']['number']['#required'])) {
    return;
  }
  // Something has been entered but is non numeric.
  if (!is_numeric($value)) {
    form_set_error('number', t('You must enter a valid phone number'));
  }
  // Something that looks like a number, verify it's not already taken.
  elseif (twilio_verify_number($value)) {
    form_set_error('number', t('This number is already in use and cannot be assigned to more than one account'));
  }
}

/**
 * Custom submit handler for phone numbers during registration.
 */
function twilio_register_submit($form, &$form_state) {
  $value = $form_state['values']['number'];
  // Phone number is not required and not entered.
  if (empty($value) && empty($form['account']['number']['#required'])) {
    return;
  }
  else {
    $account = user_load($form_state['values']['uid']);
    twilio_user_send_confirmation($account, $form_state['values']['number'], $form_state['values']['countrycode']);
  }
}

/**
 * Send confirmation message.
 *
 * @param object $account
 *   The user object of the account to message
 *
 * @param string $number
 *   The phone number to send the message
 *
 * @param string $country
 *   The country code for the number
 *
 * @todo Please document this function.
 * @see http://drupal.org/node/1354
 */
function twilio_user_send_confirmation($account, $number, $country) {
  $code = rand(1000, 9999);
  $data = array(
    'uid' => $account->uid,
    'number' => $number,
    'country' => $country,
    'status' => TWILIO_USER_PENDING,
    'code' => $code,
  );

  $account = user_save($account, array('twilio' => $data), 'twilio');
  $message = "Confirmation code: $code";
  twilio_send($number, $message, $country);
  return $account;
}
