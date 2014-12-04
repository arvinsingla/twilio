<?php
/**
 * @file
 * Rules module integration
 */

/**
 * Implements hook_rules_action_info().
 */
function twilio_twiml_rules_action_info() {
  $defaults = array(
    'group' => t('Twilio'),
    'module' => 'twilio_twiml',
  );
  $actions['twilio_twiml_output'] = $defaults + array(
    'label' => t('Output TwiML'),
    'base' => 'twilio_twiml_rules_action_twiml_output',
    'parameter' => array(
      'twiml_id' => array(
        'type' => 'text',
        'label' => t('TwiML'),
        'options list' => 'twilio_twiml_options_list',
        'restriction' => 'input',
      ),
    ),
  );
  return $actions;
}

/**
 * Action: Output TwiML.
 */
function twilio_twiml_rules_action_twiml_output($twiml_id) {
  // Load our TwiML.
  $twiml = twilio_twiml_load($twiml_id);
  // Call for output of the TwiML.
  twilio_twiml_display($twiml);
}
